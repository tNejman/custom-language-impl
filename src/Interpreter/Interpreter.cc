#include "Interpreter/Interpreter.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <variant>

#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/InterpreterHelper.hpp"
#include "Interpreter/RuntimeValue.hpp"
#include "Interpreter/ValueEvaluator.hpp"
#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

/* Environment
  private

*/

std::optional<std::reference_wrapper<const FunctionSymbol>> Interpreter::Environment::getFunctionByIdentifier(
    const std::string_view identifier ) const noexcept {
  auto it = std::ranges::find_if( functions_, [&]( const auto& function_symbol ) {
    return std::visit( interpreter_helper::Overloaded{
                           [&identifier]( const FunctionDefNode& node ) { return node.getIdentifier() == identifier; },
                           [&]( const BuiltinFunction& builtin ) { return builtin.getIdentifier() == identifier; } },
                       function_symbol );
  } );

  if ( it == functions_.end() ) {
    return std::nullopt;
  }
  return *it;
}

std::optional<std::reference_wrapper<const FunctionSymbol>> Interpreter::Environment::getFunctionBySignature(
    const std::string_view identifier, const Type& return_type,
    const std::vector<ParameterDecl>& parameters ) const noexcept {
  if ( identifier == "write" ) {
    return getFunctionByIdentifier( "write" );
  }
  auto it = std::ranges::find_if( functions_, [&]( const auto& function_symbol ) {
    return std::visit( interpreter_helper::Overloaded{ [&]( const FunctionDefNode& node ) {
                                                        return node.getIdentifier() == identifier
                                                               && node.getType() == return_type
                                                               && node.getParameters() == parameters;
                                                      },
                                                       [&]( const BuiltinFunction& builtin ) {
                                                         return builtin.getIdentifier() == identifier
                                                                && builtin.getReturnType() == return_type
                                                                && builtin.getParameters() == parameters;
                                                       } },
                       function_symbol );
  } );

  if ( it == functions_.end() ) {
    return std::nullopt;
  }
  return *it;
}

std::optional<std::reference_wrapper<const FunctionSymbol>> Interpreter::Environment::getFunctionBySignature(
    const BuiltinFunction& func ) const noexcept {
  return getFunctionBySignature( func.getIdentifier(), func.getReturnType(), func.getParameters() );
}

std::optional<std::reference_wrapper<const FunctionSymbol>> Interpreter::Environment::getFunctionBySignature(
    const FunctionDefNode& node ) const noexcept {
  return getFunctionBySignature( node.getIdentifier(), node.getType(), node.getParameters() );
}

/* Environment
  public


 */

bool Interpreter::Environment::tryAddUserFunction( const FunctionDefNode& node ) {
  if ( getFunctionBySignature( node ) ) {
    return false;
  }
  functions_.push_back( std::ref( node ) );
  return true;
}
bool Interpreter::Environment::tryAddBuiltinFunction( BuiltinFunction function ) {
  if ( getFunctionBySignature( function ) ) {
    return false;
  }
  functions_.push_back( std::move( function ) );
  return true;
}

bool Interpreter::Environment::tryAddVarOrConst( Variable variable ) {
  if ( getVarByNameThisScopeOnly( variable.getIdentifier() ) ) {
    return false;
  }
  call_stack_.top().addVariable( std::move( variable ) );
  return true;
}

std::optional<std::reference_wrapper<const Variable>> Interpreter::Environment::getVarByName(
    const std::string_view identifier ) const noexcept {
  const size_t size = call_stack_.size();

  for ( int idx = static_cast<int>( size ) - 1; idx >= 0; --idx ) {
    auto context_wrap = call_stack_.nth( static_cast<size_t>( idx ) );
    assert( context_wrap && "nth CallContext from size must exist" );
    auto& call_context = context_wrap->get();
    auto& variables = call_context.getVariables();
    auto it = std::find_if( variables.begin(), variables.end(),
                            [&]( const Variable& var ) { return var.getIdentifier() == identifier; } );

    if ( it != variables.end() ) {
      return std::ref( *it );
    }
    if ( call_context.getType() == CallContext::ContextType::FUNCTION_CALL ) {
      // cannot look beyond fun call but must check global
      if ( idx > 0 ) {
        auto& global_variables = call_stack_.nth( 0 )->get().getVariables();
        auto global_it = std::find_if( global_variables.begin(), global_variables.end(),
                                       [&]( const Variable& var ) { return var.getIdentifier() == identifier; } );

        if ( global_it != global_variables.end() ) {
          return std::ref( *global_it );
        }
      }
      break;
    }
  }

  return std::nullopt;
}
std::optional<std::reference_wrapper<Variable>> Interpreter::Environment::getVarByName(
    const std::string_view identifier ) noexcept {
  auto res = std::as_const( *this ).getVarByName( identifier );
  if ( !res ) {
    return std::nullopt;
  }
  return std::ref( const_cast<Variable&>( res->get() ) );
}
std::optional<std::reference_wrapper<const Variable>> Interpreter::Environment::getVarByNameThisScopeOnly(
    const std::string_view identifier ) const noexcept {
  const auto& cur_context_variables = call_stack_.top().getVariables();
  auto it = std::find_if( cur_context_variables.begin(), cur_context_variables.end(),
                          [&]( const Variable& var ) { return var.getIdentifier() == identifier; } );

  if ( it != cur_context_variables.end() ) {
    return std::ref( *it );
  }
  return std::nullopt;
}
std::optional<std::reference_wrapper<Variable>> Interpreter::Environment::getVarByNameThisScopeOnly(
    const std::string_view identifier ) noexcept {
  auto res = std::as_const( *this ).getVarByNameThisScopeOnly( identifier );
  if ( !res ) {
    return std::nullopt;
  }
  return std::ref( const_cast<Variable&>( res->get() ) );
}

RuntimeValue Interpreter::Environment::getRecentValFromAcc() {
  if ( accumulator_.empty() ) {
    throw std::runtime_error( "tried fetching from empty accumulator" );
  }
  auto value_popped = std::move( accumulator_.top() );
  accumulator_.pop();
  return value_popped;
}
void Interpreter::Environment::putValInAcc( RuntimeValue val ) noexcept {
  accumulator_.push( std::move( val ) );
}
size_t Interpreter::Environment::getAccSize() const noexcept {
  return accumulator_.size();
}
void Interpreter::Environment::addCallContext( CallContext::ContextType context_type ) {
  call_stack_.push( CallContext{ context_type } );
}
void Interpreter::Environment::addCallContext( const FunctionDefNode& func_def ) {
  call_stack_.push( CallContext{ func_def } );
}

void Interpreter::Environment::popLastCallContext() {
  if ( call_stack_.empty() ) {
    throw std::runtime_error( "tried popping but call stack is empty" );
  }
  call_stack_.pop();
}

bool Interpreter::Environment::isStateInWhileLoop() const noexcept {
  for ( auto it = call_stack_.view().end() - 1; it >= call_stack_.view().begin(); --it ) {
    switch ( it->getType() ) {
      case CallContext::ContextType::TOP_LEVEL: return false;
      case CallContext::ContextType::IF_BLOCK: continue;
      case CallContext::ContextType::FUNCTION_CALL: return false;
      case CallContext::ContextType::WHILE_BLOCK: return true;
    }
  }
  return false;
}
bool Interpreter::Environment::isStateInFunctionBody() const noexcept {
  for ( auto it = call_stack_.view().end() - 1; it >= call_stack_.view().begin(); --it ) {
    switch ( it->getType() ) {
      case CallContext::ContextType::TOP_LEVEL: return false;
      case CallContext::ContextType::IF_BLOCK: continue;
      case CallContext::ContextType::FUNCTION_CALL: return true;
      case CallContext::ContextType::WHILE_BLOCK: continue;
    }
  }
  return false;
}
const Type& Interpreter::Environment::getCurrentFunctionReturnType() const {
  for ( auto it = call_stack_.view().end() - 1; it >= call_stack_.view().begin(); --it ) {
    if ( it->getType() == CallContext::ContextType::FUNCTION_CALL ) {
      if ( const auto sig = it->getFunctionSig() ) {
        return sig.value().get().getType();
      }
    }
  }
  throw std::runtime_error( "cannot retriever type; not in function" );
}

bool Interpreter::Environment::matchFunctionSignature( const std::vector<ParameterDecl>& params,
                                                       const std::vector<RuntimeValue>& call_args ) const noexcept {
  if ( params.size() != call_args.size() ) {
    return false;
  }
  for ( const auto& [param, call_arg] : std::views::zip( params, call_args ) ) {
    if ( param.getMutability() != call_arg.getMutability()
         || param.getType() != interpreter_helper::getRuntimeValueType( call_arg ) ) {
      return false;
    }
    if ( param.getPassMode() == PassMode::REFERENCE ) {
      std::visit( Overloaded{ []( const RValue& _ ) { throw std::runtime_error( "cannot get reference to rvalue" ); },
                              []( const LValue& _ ) {}, []( const IndexRef& _ ) {},
                              []( Void ) { throw std::runtime_error( "cannot pass void as call arg" ); } },
                  call_arg.getData() );
    }
  }
  return true;
}
Variable Interpreter::Environment::buildVarFromParam( RuntimeValue& runtime_val, const ParameterDecl& param ) noexcept {
  switch ( param.getPassMode() ) {
    case PassMode::COPY:
      return std::visit(
          Overloaded{ [&]( const RValue& val ) -> Variable {
                       return Variable{ param.getIdentifier(), param.getType().copy(), param.getMutability(),
                                        std::make_shared<Value>( val.copy() ) };
                     },
                      [&]( const LValue& val ) -> Variable {
                        return Variable{ param.getIdentifier(), param.getType().copy(), param.getMutability(),
                                         std::make_shared<Value>( val.get().getValue()->copy() ) };
                      },
                      [&]( const IndexRef& val ) -> Variable {
                        return Variable{ param.getIdentifier(), param.getType().copy(), param.getMutability(),
                                         std::make_shared<Value>( val->copy() ) };
                      },
                      [&]( Void ) -> Variable { throw std::runtime_error( "cannot build variable from void" ); } },
          runtime_val.getData() );
    case PassMode::REFERENCE:
      return std::visit(
          Overloaded{
              [&]( const RValue& _ ) -> Variable { throw std::runtime_error( "cannot make reference to an rvalue" ); },
              [&]( const LValue& val ) -> Variable {
                return Variable{ param.getIdentifier(), param.getType().copy(), param.getMutability(),
                                 val.get().getValue() };
              },
              [&]( const IndexRef& val ) -> Variable {
                return Variable{ param.getIdentifier(), param.getType().copy(), param.getMutability(), val };
              },
              [&]( Void ) -> Variable { throw std::runtime_error( "cannot build variable from void" ); } },
          runtime_val.getData() );
  }
}

/* Interpreter
  private


 */

void Interpreter::handleStatementList( const std::vector<std::unique_ptr<INode>>& statements,
                                       CallContext::ContextType context_type ) {
  CallContextGuard guard{ environment_, context_type };
  for ( const auto& stmt_ptr : statements ) {
    const size_t initial_acc_size = environment_.getAccSize();
    stmt_ptr->accept( *this );
    /* statement may leave value in acc, e.g.
    x = 4
    leaves Void; cleanup needed to avoid overflow
    */
    if ( environment_.getAccSize() > initial_acc_size ) {
      environment_.getRecentValFromAcc();
    }
    assert( environment_.getAccSize() == initial_acc_size && "accumulator mustn't grow after statement execution" );
  }
}

/* Interpreter
 public



*/

Interpreter::Interpreter( std::unique_ptr<const ProgramNode> program ) : program_( std::move( program ) ) {
  if ( !environment_.tryAddBuiltinFunction( BuiltinFunction{
           "write", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ), {}, builtin_functions::write } ) ) {
    throw std::runtime_error( "couldnt add 'write' to builtin functions" );
  }
  if ( !environment_.tryAddBuiltinFunction( BuiltinFunction{
           "read", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
           [] {
             std::vector<ParameterDecl> params;
             params.push_back( ParameterDecl{ "prompt", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
                                              PassMode::REFERENCE, Mutability::IMMUTABLE } );
             return params;
           }(),
           builtin_functions::read } ) ) {
    throw std::runtime_error( "couldnt add 'read' to builtin functions" );
  }
  if ( !environment_.tryAddBuiltinFunction( BuiltinFunction{
           "exit", BaseType::INT,
           [] {
             std::vector<ParameterDecl> params;
             params.push_back( ParameterDecl{ "code", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } );
             return params;
           }(),
           builtin_functions::exit } ) ) {
    throw std::runtime_error( "couldnt add 'exit' to builtin functions" );
  }
}

void Interpreter::execute() {
  this->program_->accept( *this );
}

void Interpreter::visit( const FunctionDefNode& node ) {
  CallContextGuard guard{ environment_, node };

  std::vector<RuntimeValue> call_args;
  call_args.reserve( node.getParameters().size() );
  for ( const auto& _ : node.getParameters() ) {
    call_args.push_back( environment_.getRecentValFromAcc() );
  }
  std::reverse( call_args.begin(), call_args.end() );

  if ( !environment_.matchFunctionSignature( node.getParameters(), call_args ) ) {
    throw std::runtime_error( "call args don't match function signature" );
  }
  for ( const auto& [runtime_value, param] : std::ranges::views::zip( call_args, node.getParameters() ) ) {
    if ( !environment_.tryAddVarOrConst( environment_.buildVarFromParam( runtime_value, param ) ) ) {
      throw std::runtime_error( "shadowing a variable in the same scope" );
    }
  }
  RuntimeValue ret_val{};
  for ( const auto& stmt : node.getBlock() ) {
    const size_t org_acc_size = environment_.getAccSize();
    stmt->accept( *this );
    if ( environment_.getFlowControlType() == Environment::ControlFlow::RETURN ) {
      ret_val = environment_.getRecentValFromAcc();
      break;
    }
    environment_.getRecentValFromAcc();
    assert( environment_.getAccSize() == org_acc_size
            && "after statement stack must remain same size (including cleanup)" );
  }
  if ( interpreter_helper::isVoidFunction( node ) ) {
    if ( ret_val ) {
      throw std::runtime_error( "void function returned value" );
    }
    environment_.putValInAcc( RuntimeValue{} );
    return;
  }
  if ( !ret_val ) {
    throw std::runtime_error( "non-void function did not return value" );
  }
  if ( interpreter_helper::getRuntimeValueType( ret_val ) != node.getType() ) {
    throw std::runtime_error( "returned value does not match function return type" );
  }
  environment_.putValInAcc( std::move( ret_val ) );
}

void Interpreter::visit( const VarOrConstDeclNode& node ) {
  node.getInitializerExpr()->accept( *this );
  RuntimeValue runtime_val = environment_.getRecentValFromAcc();
  if ( !runtime_val ) {
    throw std::runtime_error( "cannot assign 'void' to a variable" );
  }
  Value assigned_value = interpreter_helper::extractRuntimeValue( runtime_val );
  if ( node.getType() != assigned_value.getValueType() ) {
    throw std::runtime_error( "initializer value does not match variable type" );
  }
  if ( !environment_.tryAddVarOrConst( Variable{ std::string( node.getIdentifier() ), node.getType().copy(),
                                                 node.getMutability(),
                                                 std::make_shared<Value>( assigned_value.copy() ) } ) ) {
    throw std::runtime_error( "duplicated variable at current scope" );
  };
}

void Interpreter::visit( const IfStatementNode& node ) {
  auto evaluate_cond = [&]() {
    try {
      return interpreter_helper::evaluateRuntimeValue<bool>( environment_.getRecentValFromAcc() );
    } catch ( const std::bad_variant_access& ) {
      throw std::runtime_error( "if condition must be type bool" );
    }
  };

  bool any_block_entered = false;
  for ( const auto& [cond, block] : node.getCondBlockPairs() ) {
    cond->accept( *this );
    if ( evaluate_cond() ) {
      handleStatementList( block, CallContext::ContextType::IF_BLOCK );
      any_block_entered = true;
      break;
    }
  }
  if ( !any_block_entered ) {
    handleStatementList( node.getElseBlock(), CallContext::ContextType::IF_BLOCK );
  }
}

void Interpreter::visit( const WhileStatementNode& node ) {
  auto evaluate_condition = [&] -> bool {
    try {
      node.getCondition()->accept( *this );
      RuntimeValue runtime_condition_val = environment_.getRecentValFromAcc();
      return interpreter_helper::evaluateRuntimeValue<bool>( runtime_condition_val );
    } catch ( const std::bad_variant_access& ) {
      throw std::runtime_error( "while condition must be type bool" );
    }
  };
  while ( evaluate_condition() ) {
    CallContextGuard guard{ environment_, CallContext::ContextType::WHILE_BLOCK };
    bool should_break = false;
    for ( const auto& stmt : node.getBlock() ) {
      stmt->accept( *this );
      auto ctrl = environment_.getFlowControlType();
      if ( ctrl != Environment::ControlFlow::NONE ) {
        should_break = ( ctrl == Environment::ControlFlow::BREAK || ctrl == Environment::ControlFlow::RETURN );
        if ( ctrl != Environment::ControlFlow::RETURN ) {
          environment_.setFlowControlType( Environment::ControlFlow::NONE );
        }
        break;
      }
    }
    if ( should_break ) break;
  }
}

void Interpreter::visit( const ControlFlowNode& node ) {
  if ( !environment_.isStateInWhileLoop() ) {
    throw std::runtime_error( "loop control not in a loop" );
  }
  switch ( node.getControlFlowType() ) {
    case ControlFlowType::BREAK: environment_.setFlowControlType( Environment::ControlFlow::BREAK ); return;
    case ControlFlowType::CONTINUE: environment_.setFlowControlType( Environment::ControlFlow::CONTINUE ); return;
  }
}

void Interpreter::visit( const ReturnNode& node ) {
  if ( !environment_.isStateInFunctionBody() ) {
    throw std::runtime_error( "return statement not in a function" );
  }
  if ( !node.getExpression() ) {
    environment_.putValInAcc( RuntimeValue{} );
    return;
  }
  node.getExpression()->accept( *this );
  RuntimeValue ret_runtime_val = environment_.getRecentValFromAcc();
  Value ret_val = interpreter_helper::extractRuntimeValue( ret_runtime_val );
  environment_.putValInAcc( RuntimeValue{ std::move( ret_val ) } );
}

void Interpreter::visit( const AssignmentExprNode& node ) {
  node.getLeftOperand()->accept( *this );
  auto left_val_from_acc = environment_.getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  auto right_val_from_acc = environment_.getRecentValFromAcc();

  if ( interpreter_helper::getRuntimeValueType( left_val_from_acc )
       != interpreter_helper::getRuntimeValueType( right_val_from_acc ) ) {
    throw std::runtime_error( "assignment operand must match type" );
  }
  if ( !left_val_from_acc.isAssignableTo() ) {
    throw std::runtime_error( "cannot assign to immutable value" );
  }
  Value& val_assigned_to = [&]( const auto& rt_val_ins ) -> Value& {
    using T = std::decay_t<decltype( rt_val_ins )>;
    if constexpr ( std::is_same_v<T, LValue> ) {
      return rt_val_ins.get().getValue();
    } else if constexpr ( std::is_same_v<T, IndexRef> ) {
      return *rt_val_ins;
    }
    throw std::runtime_error( "check type failed" );
  }( left_val_from_acc );

  Value val_assigned_from = interpreter_helper::extractRuntimeValue( right_val_from_acc );

  switch ( node.getAssignmentType() ) {
    case AssignmentType::ASSIGN: val_assigned_to = std::move( val_assigned_from ); break;
    case AssignmentType::ADD_ASSIGN:
      val_assigned_to = ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, std::plus<>{} );
    case AssignmentType::SUB_ASSIGN:
      val_assigned_to = ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, std::minus<>{} );
    case AssignmentType::MUL_ASSIGN:
      val_assigned_to = ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, std::multiplies<>{} );
    case AssignmentType::DIV_ASSIGN:
      val_assigned_to = ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, std::divides<>{} );
    case AssignmentType::MOD_ASSIGN:
      val_assigned_to =
          ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, ValueEvaluator::FmodModulus{} );
  }
  environment_.putValInAcc( RuntimeValue{} );
}

void Interpreter::visit( const BinaryExprNode& node ) {
  node.getLeftOperand()->accept( *this );
  RuntimeValue left_operand_rt_val = environment_.getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  RuntimeValue right_operand_rt_val = environment_.getRecentValFromAcc();

  if ( interpreter_helper::getRuntimeValueType( left_operand_rt_val )
       != interpreter_helper::getRuntimeValueType( right_operand_rt_val ) ) {
    throw std::runtime_error( "type mismatch in binary expr" );
  }
  Value l_operand = interpreter_helper::extractRuntimeValue( left_operand_rt_val );
  Value r_operand = interpreter_helper::extractRuntimeValue( right_operand_rt_val );

  switch ( node.getOperator() ) {
    case BinaryOperator::AND: {
      environment_.putValInAcc(
          RuntimeValue{ std::get<bool>( l_operand.getData() ) && std::get<bool>( r_operand.getData() ) } );
      return;
    }
    case BinaryOperator::OR: {
      environment_.putValInAcc(
          RuntimeValue{ std::get<bool>( l_operand.getData() ) || std::get<bool>( r_operand.getData() ) } );
      return;
    }
    case BinaryOperator::EQ: environment_.putValInAcc( RuntimeValue{ l_operand == r_operand } ); return;
    case BinaryOperator::NEQ: environment_.putValInAcc( RuntimeValue{ l_operand != r_operand } ); return;
    case BinaryOperator::LT:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::less<>{} ) } );
      return;
    case BinaryOperator::LEQ:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::less_equal<>{} ) } );
      return;
    case BinaryOperator::GT:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::greater<>{} ) } );
      return;
    case BinaryOperator::GEQ:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::greater_equal<>{} ) } );
      return;
    case BinaryOperator::ADD:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::plus<>{} ) } );
      return;
    case BinaryOperator::SUB:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::minus<>{} ) } );
      return;
    case BinaryOperator::MUL:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::multiplies<>{} ) } );
      return;
    case BinaryOperator::DIV:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::divides<>{} ) } );
      return;
    case BinaryOperator::MOD:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::FmodModulus{} ) } );
      return;
    case BinaryOperator::CONCAT:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateArrOp( l_operand, r_operand, ArrayBinaryOperator::CONCAT ) } );
      return;
    case BinaryOperator::DIFF:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateArrOp( l_operand, r_operand, ArrayBinaryOperator::DIFF ) } );
      return;
    case BinaryOperator::ACCESS:
      environment_.putValInAcc( RuntimeValue{ ValueEvaluator::evaluateArrAccess( l_operand, r_operand ) } );
      return;
    case BinaryOperator::MAP:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateArrOpSpecial( l_operand, r_operand, ArrayIdentifierOpType::MAP ) } );
      return;
    case BinaryOperator::FILTER:
      environment_.putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateArrOpSpecial( l_operand, r_operand, ArrayIdentifierOpType::FILTER ) } );
      return;
  }
}

void Interpreter::visit( const UnaryExprNode& node ) {
  node.getOperand()->accept( *this );
  RuntimeValue operand_rt_val = environment_.getRecentValFromAcc();
  if ( !operand_rt_val ) {
    throw std::runtime_error( "cannot perform unary op on void" );
  }
  Value operand_val = interpreter_helper::extractRuntimeValue( operand_rt_val );
  environment_.putValInAcc( RuntimeValue{ ValueEvaluator::evaluateUnaryOp( operand_val, node.getOperator() ) } );
}

void Interpreter::visit( const CastExprNode& node ) {
  node.getExpression()->accept( *this );
  RuntimeValue operand_rt_val = environment_.getRecentValFromAcc();
  if ( !operand_rt_val ) {
    throw std::runtime_error( "cannot cast void" );
  }
  Value operand_val = interpreter_helper::extractRuntimeValue( operand_rt_val );
  environment_.putValInAcc( RuntimeValue{ ValueEvaluator::evaluateCastOp( operand_val, node.getType() ) } );
}

void Interpreter::visit( const FunctionCallNode& node ) {
  std::vector<RuntimeValue> call_args;
  call_args.reserve( node.getArguments().size() );
  for ( const auto& call_arg_decl : node.getArguments() ) {
    call_arg_decl->accept( *this );
    RuntimeValue arg_val = environment_.getRecentValFromAcc();
    if ( !arg_val ) {
      throw std::runtime_error( "'void' is not a valid call arg" );
    }
    call_args.push_back( std::move( arg_val ) );
  }
  if ( auto fun = environment_.getFunctionBySignature( node.getIdentifier(), call_args ) ) {
    std::visit( Overloaded{ [&]( std::reference_wrapper<const FunctionDefNode> fun_def_node ) {
                             for ( auto& call_arg_val : call_args ) {
                               environment_.putValInAcc( std::move( call_arg_val ) );
                             }
                             fun_def_node.get().accept( *this );
                           },
                            [&]( const BuiltinFunction& b_fun ) {
                              std::vector<Value> call_args_eval;
                              call_args_eval.reserve( call_args.size() );
                              for ( auto& call_arg : call_args ) {
                                call_args_eval.push_back( interpreter_helper::extractRuntimeValue( call_arg ) );
                              }
                              b_fun.getMappedFunction()( call_args_eval );
                            } },
                fun->get() );
  }
}

void Interpreter::visit( const ArrayLiteralNode& node ) {
  assert( node.getPositions().size() > 0 && "empty literal is illegal" );
  std::vector<Value> array_positions;
  array_positions.reserve( node.getPositions().size() );
  node.getPositions()[0]->accept( *this );
  RuntimeValue first_val = environment_.getRecentValFromAcc();
  if ( !first_val ) {
    throw std::runtime_error( "array literal position cannot be void" );
  }
  const Type& deduced_type = interpreter_helper::getRuntimeValueType( first_val );
  array_positions.push_back( interpreter_helper::extractRuntimeValue( first_val ) );

  for ( auto i = 1u; i < node.getPositions().size(); ++i ) {
    node.getPositions()[i]->accept( *this );
    RuntimeValue nth_value = environment_.getRecentValFromAcc();
    if ( !nth_value ) {
      throw std::runtime_error( "array literal n cannot be void" );
    }
    if ( interpreter_helper::getRuntimeValueType( nth_value ) != deduced_type ) {
      throw std::runtime_error( "array must be homogenous" );
    }
    array_positions.push_back( interpreter_helper::extractRuntimeValue( nth_value ) );
  }
  Value composed_array{ std::move( array_positions ) };
  environment_.putValInAcc( RuntimeValue{ std::move( composed_array ) } );
}

void Interpreter::visit( const LiteralExprNode& node ) {
  environment_.putValInAcc( RuntimeValue{ node.getValue().copy() } );
}

void Interpreter::visit( const PrimaryIdentifierNode& node ) {
  if ( auto var = environment_.getVarByName( node.getIdentifier() ) ) {
    environment_.putValInAcc( RuntimeValue{ var->get() } );
  }
  throw std::runtime_error( "usage of unknown variable" );
}

void Interpreter::visit( const ProgramNode& node ) {
  for ( const auto& func_def_ptr : node.getFunctionList() ) {
    if ( !environment_.tryAddUserFunction( *func_def_ptr ) ) {
      throw std::runtime_error( "duplicated function signature" );
    }
  }
  handleStatementList( node.getStatementList(), CallContext::ContextType::TOP_LEVEL );
  throw std::runtime_error( "check if call stack and acc empty after program execution" );
}
