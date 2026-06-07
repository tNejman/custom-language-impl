#include "Interpreter/Interpreter.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <ranges>
#include <variant>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/RuntimeValue.h"
#include "Interpreter/ValueEvaluator.hpp"
#include "Interpreter/Variable.h"
#include "Parser/IFunction.hpp"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

/* Environment
  public

*/

// std::vector<std::reference_wrapper<const IFunction>> Interpreter::Environment::getFunctionByIdentifier(
//     const std::string_view identifier ) const noexcept {
//   return functions_
//          | std::views::filter( [&identifier]( const IFunction& ifun ) { return ifun.getIdentifier() == identifier; }
//          ) | std::ranges::to<std::vector>();
// }

// std::optional<std::reference_wrapper<const BuiltinFunction>>
// Interpreter::Environment::getBuiltinFunctionByIdentifier(
//     const std::string identifier ) const noexcept {
//   auto it = std::ranges::find_if( builtin_storage_,
//                                   [&identifier]( const auto& fun ) { return fun.getIdentifier() == identifier; } );

//   if ( it == builtin_storage_.end() ) {
//     return std::nullopt;
//   }
//   return *it;
// }

std::optional<std::reference_wrapper<const IFunction>> Interpreter::Environment::getFunctionBySignature(
    const std::string_view identifier, const std::vector<ParameterDecl>& parameters ) noexcept {
  if ( identifier == "write" ) {
    tryAddBuiltinFunction( builtin_functions::buildBuiltinWrite( parameters ) );
  }
  // auto it = std::ranges::find_if( functions_, [&]( const IFunction& fun ) {
  //   return fun.getIdentifier() == identifier && fun.getParameters() == parameters;
  // } );
  // if ( it == functions_.end() ) {
  //   return std::nullopt;
  // }
  // return *it;
  for ( const auto& fun : functions_ ) {
    if ( fun.get().getIdentifier() == identifier && fun.get().getParameters() == parameters ) {
      return fun;
    }
  }
  return std::nullopt;
}

std::optional<std::reference_wrapper<const IFunction>> Interpreter::Environment::getFunctionBySignature(
    const std::string_view identifier, const std::vector<RuntimeValue>& call_args ) noexcept {
  if ( identifier == "write" ) {
    tryAddBuiltinFunction( builtin_functions::buildBuiltinWrite( call_args ) );
  }
  auto it = std::ranges::find_if(
      functions_, [&]( const IFunction& fun ) { return matchFunctionSignature( fun.getParameters(), call_args ); } );
  if ( it == functions_.end() ) {
    return std::nullopt;
  }
  return *it;
}

bool Interpreter::Environment::tryAddUserFunction( const FunctionDefNode& node ) {
  if ( getFunctionBySignature( node.getIdentifier(), node.getParameters() ) ) {
    return false;
  }
  functions_.push_back( std::ref( node ) );
  return true;
}
bool Interpreter::Environment::tryAddBuiltinFunction( BuiltinFunction function ) {
  if ( getFunctionBySignature( function.getIdentifier(), function.getParameters() ) ) {
    return false;
  }
  auto& func_ref = builtin_storage_.emplace_back( std::move( function ) );
  functions_.push_back( func_ref );
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

bool Interpreter::Environment::matchFunctionSignature( const std::vector<ParameterDecl>& params,
                                                       const std::vector<RuntimeValue>& call_args ) const noexcept {
  if ( params.size() != call_args.size() ) {
    return false;
  }
  for ( auto&& [param, call_arg] : std::views::zip( params, call_args ) ) {
    if ( param.getMutability() != call_arg.getMutability() || param.getType() != call_arg.getType() ) {
      return false;
    }
    if ( param.getPassMode() == PassMode::REFERENCE ) {
      std::visit(
          Overloaded{ []( const RValue& _ ) {
                       throw InvalidAccessException( Position{ 1, 1 }, "cannot get reference to rvalue" );
                     },
                      []( const LValue& _ ) {}, []( const IndexRef& _ ) {},
                      []( Void ) { throw VoidValueException( Position{ 1, 1 }, "cannot pass void as call arg" ); } },
          call_arg.peekData() );
    }
  }
  return true;
}
Interpreter::Environment::ControlFlow Interpreter::Environment::getFlowControlType() const noexcept {
  return loop_control_type_;
}
void Interpreter::Environment::setFlowControlType( ControlFlow control_flow ) noexcept {
  loop_control_type_ = control_flow;
}

/* Interpreter
  private


 */

RuntimeValue Interpreter::getRecentValFromAcc() {
  if ( accumulator_.empty() ) {
    throw EmptyAccumulatorException( Position{ 1, 1 }, "tried fetching from empty accumulator" );
  }
  auto value_popped = std::move( accumulator_.top() );
  accumulator_.pop();
  return value_popped;
}
void Interpreter::putValInAcc( RuntimeValue val ) noexcept {
  accumulator_.push( std::move( val ) );
}

Variable Interpreter::buildVarFromParam( RuntimeValue& runtime_val, const ParameterDecl& param ) noexcept {
  switch ( param.getPassMode() ) {
    case PassMode::COPY:
      return std::visit( Overloaded{ [&]( const RValue& val ) -> Variable {
                                      return Variable{ param.getIdentifier(), param.getType().copy(),
                                                       param.getMutability(), std::make_shared<Value>( val.copy() ) };
                                    },
                                     [&]( const LValue& val ) -> Variable {
                                       return Variable{ param.getIdentifier(), param.getType().copy(),
                                                        param.getMutability(),
                                                        std::make_shared<Value>( val.get().getValue()->copy() ) };
                                     },
                                     [&]( const IndexRef& val ) -> Variable {
                                       return Variable{ param.getIdentifier(), param.getType().copy(),
                                                        param.getMutability(), val.get().copy() };
                                     },
                                     [&]( Void ) -> Variable {
                                       throw VoidValueException( Position{ 1, 1 }, "cannot build variable from void" );
                                     } },
                         runtime_val.peekData() );
    case PassMode::REFERENCE:
      return std::visit( Overloaded{ [&]( const RValue& _ ) -> Variable {
                                      throw InvalidAccessException( Position{ 1, 1 },
                                                                    "cannot make reference to an rvalue" );
                                    },
                                     [&]( const LValue& val ) -> Variable {
                                       return Variable{ param.getIdentifier(), param.getType().copy(),
                                                        param.getMutability(), val.get().getValue() };
                                     },
                                     [&]( const IndexRef& val ) -> Variable {
                                       return Variable{ param.getIdentifier(), param.getType().copy(),
                                                        param.getMutability(), val.get().copy() };
                                     },
                                     [&]( Void ) -> Variable {
                                       throw VoidValueException( Position{ 1, 1 }, "cannot build variable from void" );
                                     } },
                         runtime_val.peekData() );
  }
}

void Interpreter::handleStatementList( const std::vector<std::unique_ptr<INode>>& statements,
                                       CallContext::ContextType context_type ) {
  CallContextGuard cc_guard{ environment_, context_type, statements.size() };
  for ( const auto& stmt_ptr : statements ) {
    // if ( debug_hook_ ) {
    //   debug_hook_( *this, *stmt_ptr );
    // }
    AccumulatorGuard acc_guard{ accumulator_ };
    stmt_ptr->accept( *this );
    if (environment_.getFlowControlType() != Environment::ControlFlow::NONE) {
      break;
    }
    acc_guard.validate( stmt_ptr->getPosition() );
  }
}

/* CallContextGuard



*/

inline Interpreter::CallContextGuard::CallContextGuard( Environment& env, CallContext::ContextType context_type,
                                                        size_t expect_variable_count )
    : env_( env ) {
  env.call_stack_.push( CallContext{ context_type, expect_variable_count } );
}
inline Interpreter::CallContextGuard::CallContextGuard( Environment& env, const FunctionDefNode& func_def,
                                                        size_t expect_variable_count )
    : env_( env ) {
  env.call_stack_.push( CallContext{ func_def, expect_variable_count } );
}
inline Interpreter::CallContextGuard::~CallContextGuard() {
  if ( !env_.call_stack_.empty() ) {
    env_.call_stack_.pop();
  }
}

/* AccumulatorGuard



*/

Interpreter::AccumulatorGuard::AccumulatorGuard( std::stack<RuntimeValue>& acc ) noexcept
    : acc_( acc ), org_size_( acc.size() ) {
}
void Interpreter::AccumulatorGuard::validate( Position pos ) {
  assert( acc_.size() == org_size_ + 1u && "last statement didn't leave value in acc" );
  if ( !acc_.top().isVoid() ) {
    throw InvalidStatementException( pos, "last statement was non-void" );
  }
}
Interpreter::AccumulatorGuard::~AccumulatorGuard() noexcept {
  if ( !acc_.empty() ) {
    acc_.pop();
  }
}

/* Debug Guard



*/
Interpreter::DebugGuard::DebugGuard( Interpreter& interpreter, const INode& node ) noexcept
    : interpreter_( interpreter ), node_( node ) {
  if ( interpreter_.debug_hook_ ) {
    interpreter_.debug_hook_( interpreter_, node_, DebugEvent::BEFORE_NODE_VISIT );
  }
}
Interpreter::DebugGuard::~DebugGuard() noexcept {
  if ( interpreter_.debug_hook_ ) {
    interpreter_.debug_hook_( interpreter_, node_, DebugEvent::AFTER_NODE_VISIT );
  }
}

/* Interpreter
 public



*/

Interpreter::Interpreter( std::unique_ptr<const ProgramNode> program ) : program_( std::move( program ) ) {
  {
    bool added_read = environment_.tryAddBuiltinFunction( BuiltinFunction{
        Position{ 99999, 99999 }, "read", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
        [] {
          std::vector<ParameterDecl> params;
          params.push_back( ParameterDecl{ "prompt", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ), PassMode::COPY,
                                           Mutability::IMMUTABLE } );
          return params;
        }(),
        builtin_functions::read } );
    assert( added_read );
  }
  {
    bool added_exit = environment_.tryAddBuiltinFunction( BuiltinFunction{
        Position{ 99999, 99999 }, "exit", BaseType::INT,
        [] {
          std::vector<ParameterDecl> params;
          params.push_back( ParameterDecl{ "code", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } );
          return params;
        }(),
        builtin_functions::exit } );
    assert( added_exit );
  }
}

void Interpreter::execute() {
  // static bool was_executed = false;
  // if ( !was_executed ) {
  this->program_->accept( *this );
  //   was_executed = true;
  // } else {
  //   throw std::runtime_error( "cannot execute twice" );
  // }
}

void Interpreter::visit( const FunctionDefNode& node ) {
  DebugGuard dbg_guard{ *this, node };
  CallContextGuard guard{ environment_, node, node.getParameters().size() + node.getBlock().size() };

  if ( node.getIdentifier() == "write" ) {
    throw InvalidOverloadException( Position{ 1, 1 },
                                    "function 'write' already available for all possible types - cannot overload" );
  }

  std::vector<RuntimeValue> call_args;
  call_args.reserve( node.getParameters().size() );
  for ( const auto& _ : node.getParameters() ) {
    call_args.push_back( getRecentValFromAcc() );
  }
  std::reverse( call_args.begin(), call_args.end() );

  if ( !environment_.matchFunctionSignature( node.getParameters(), call_args ) ) {
    throw FunctionSignatureMismatchException( Position{ 1, 1 },
                                              "call args don't match function signature" );  // dont trust function call
  }
  for ( const auto& [runtime_value, param] : std::ranges::views::zip( call_args, node.getParameters() ) ) {
    if ( !environment_.tryAddVarOrConst( buildVarFromParam( runtime_value, param ) ) ) {
      throw InvalidShadowException( node.getPosition(), "shadowing a variable in the same scope" );
    }
  }
  RuntimeValue ret_val{};
  for ( const auto& stmt : node.getBlock() ) {
    AccumulatorGuard acc_guard{ accumulator_ };
    stmt->accept( *this );
    if ( environment_.getFlowControlType() == Environment::ControlFlow::RETURN ) {
      ret_val = getRecentValFromAcc();
      environment_.setFlowControlType( Environment::ControlFlow::NONE );
      break;
    }
    acc_guard.validate( stmt->getPosition() );
  }
  if ( node.isVoid() ) {
    if ( ret_val ) {
      throw VoidValueException( Position{ 1, 1 }, "void function returned value" );
    }
    putValInAcc( RuntimeValue{} );
    return;
  }
  if ( !ret_val ) {
    throw VoidValueException( node.getPosition(), "non-void function did not return value" );
  }
  if ( ret_val.getType() != node.getReturnType() ) {
    throw NotAllowedTypeException( node.getPosition(), "returned value does not match function return type" );
  }
  putValInAcc( std::move( ret_val ) );
}

void Interpreter::visit( const VarOrConstDeclNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  node.getInitializerExpr()->accept( *this );
  RuntimeValue runtime_val = getRecentValFromAcc();
  if ( !runtime_val ) {
    throw VoidValueException( node.getPosition(), "cannot assign 'void' to a variable" );
  }
  Value assigned_value = runtime_val.extractValue();
  if ( node.getType() != assigned_value.getValueType() ) {
    throw NotAllowedTypeException( node.getPosition(), "initializer value does not match variable type" );
  }
  if ( !environment_.tryAddVarOrConst( Variable{ std::string( node.getIdentifier() ), node.getType().copy(),
                                                 node.getMutability(),
                                                 std::make_shared<Value>( assigned_value.copy() ) } ) ) {
    throw InvalidShadowException( node.getPosition(), "duplicated variable at current scope" );
  };
  putValInAcc( RuntimeValue{} );  // decl returns Void
}

void Interpreter::visit( const IfStatementNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  auto evaluate_cond = [&]() {
    try {
      RuntimeValue runtime_val = getRecentValFromAcc();
      return runtime_val.evaluateValue<bool>();
    } catch ( const std::bad_variant_access& ) {
      throw NotAllowedTypeException( node.getPosition(), "if condition must be type bool" );
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
  putValInAcc( RuntimeValue{} );  // if stmt returns Void
}

void Interpreter::visit( const WhileStatementNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  auto evaluate_condition = [&] -> bool {
    try {
      node.getCondition()->accept( *this );
      RuntimeValue runtime_condition_val = getRecentValFromAcc();
      return runtime_condition_val.evaluateValue<bool>();
    } catch ( const std::bad_variant_access& ) {
      throw NotAllowedTypeException( node.getCondition()->getPosition(), "while condition must be type bool" );
    }
  };
  while ( evaluate_condition() ) {
    CallContextGuard guard{ environment_, CallContext::ContextType::WHILE_BLOCK, node.getBlock().size() };
    bool should_break = false;
    for ( const auto& stmt : node.getBlock() ) {
      // if ( debug_hook_ ) {
      //   debug_hook_( *this, *stmt );
      // }
      AccumulatorGuard acc_guard{ accumulator_ };
      stmt->accept( *this );
      acc_guard.validate( stmt->getPosition() );
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
  putValInAcc( RuntimeValue{} );  // while stmt returns void
}

void Interpreter::visit( const ControlFlowNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  if ( !environment_.isStateInWhileLoop() ) {
    throw InvalidStatementException( node.getPosition(), "loop control not in a loop" );
  }
  switch ( node.getControlFlowType() ) {
    case ControlFlowType::BREAK: environment_.setFlowControlType( Environment::ControlFlow::BREAK ); break;
    case ControlFlowType::CONTINUE: environment_.setFlowControlType( Environment::ControlFlow::CONTINUE ); break;
  }
  putValInAcc( RuntimeValue{} );  // control flow returns void
}

void Interpreter::visit( const ReturnNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  if ( !environment_.isStateInFunctionBody() ) {
    throw InvalidStatementException( node.getPosition(), "return statement not in a function" );
  }
  if ( !node.getExpression() ) {
    putValInAcc( RuntimeValue{} );
    return;
  }
  node.getExpression()->accept( *this );
  // evaluated ret may just rest in acc; even void; caller catches
  environment_.setFlowControlType( Environment::ControlFlow::RETURN );
}

void Interpreter::visit( const AssignmentExprNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  node.getLeftOperand()->accept( *this );
  auto left_val_from_acc = getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  auto right_val_from_acc = getRecentValFromAcc();

  if ( left_val_from_acc.getType() != right_val_from_acc.getType() ) {
    throw NotAllowedTypeException( node.getPosition(), "assignment operand must match type" );
  }
  if ( !left_val_from_acc.isAssignableTo() ) {
    throw InvalidAccessException( node.getLeftOperand()->getPosition(), "cannot assign to immutable value" );
  }
  Value& val_assigned_to =
      std::visit( Overloaded{ []( const LValue& val ) -> Value& { return *( val.get().getValue() ); },
                              []( const IndexRef& iref ) -> Value& { return iref.get(); },
                              []( const auto& ) -> Value& { std::unreachable(); } },
                  left_val_from_acc.peekData() );

  Value val_assigned_from = right_val_from_acc.extractValue();

  switch ( node.getAssignmentType() ) {
    case AssignmentType::ASSIGN: val_assigned_to = std::move( val_assigned_from ); break;
    case AssignmentType::ADD_ASSIGN:
      val_assigned_to =
          ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, ValueEvaluator::NumericOp::ADD );
    case AssignmentType::SUB_ASSIGN:
      val_assigned_to =
          ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, ValueEvaluator::NumericOp::SUB );
    case AssignmentType::MUL_ASSIGN:
      val_assigned_to =
          ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, ValueEvaluator::NumericOp::MUL );
    case AssignmentType::DIV_ASSIGN:
      val_assigned_to =
          ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, ValueEvaluator::NumericOp::DIV );
    case AssignmentType::MOD_ASSIGN:
      val_assigned_to =
          ValueEvaluator::evaluateNumeric( val_assigned_to, val_assigned_from, ValueEvaluator::NumericOp::MOD );
  }
  putValInAcc( RuntimeValue{} );
}

void Interpreter::visit( const BinaryExprNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  node.getLeftOperand()->accept( *this );
  RuntimeValue left_operand_rt_val = getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  RuntimeValue right_operand_rt_val = getRecentValFromAcc();

  if ( ( left_operand_rt_val.getType() != right_operand_rt_val.getType() )
       && left_operand_rt_val.getType() != BaseType::VOID && right_operand_rt_val.getType() != BaseType::VOID
       && node.getOperator() != BinaryOperator::ACCESS ) {
    throw NotAllowedTypeException( node.getPosition(), "type mismatch in binary expr" );
  }
  Value l_operand = [&] {
    if ( node.getOperator() == BinaryOperator::ACCESS ) {
      return Value{ 0 };
    } else {
      return left_operand_rt_val.extractValue();
    }
  }();
  Value r_operand = [&] {
    if ( node.getOperator() == BinaryOperator::ACCESS ) {
      return Value{ 0 };
    } else {
      return right_operand_rt_val.extractValue();
    }
  }();
  std::println( "\nlop binexpr:{}\n", l_operand );
  std::println( "\nrop binexpr:{}\n", r_operand );

  switch ( node.getOperator() ) {
    case BinaryOperator::AND: {
      putValInAcc( RuntimeValue{ std::get<bool>( l_operand.getData() ) && std::get<bool>( r_operand.getData() ) } );
      return;
    }
    case BinaryOperator::OR: {
      putValInAcc( RuntimeValue{ std::get<bool>( l_operand.getData() ) || std::get<bool>( r_operand.getData() ) } );
      return;
    }
    case BinaryOperator::EQ: putValInAcc( RuntimeValue{ l_operand == r_operand } ); return;
    case BinaryOperator::NEQ: putValInAcc( RuntimeValue{ l_operand != r_operand } ); return;
    case BinaryOperator::LT:
      putValInAcc( RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::less<>{} ) } );
      return;
    case BinaryOperator::LEQ:
      putValInAcc( RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::less_equal<>{} ) } );
      return;
    case BinaryOperator::GT:
      putValInAcc( RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::greater<>{} ) } );
      return;
    case BinaryOperator::GEQ:
      putValInAcc( RuntimeValue{ ValueEvaluator::evaluateRelational( l_operand, r_operand, std::greater_equal<>{} ) } );
      return;
    case BinaryOperator::ADD:
      putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::NumericOp::ADD ) } );
      return;
    case BinaryOperator::SUB:
      putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::NumericOp::SUB ) } );
      return;
    case BinaryOperator::MUL:
      putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::NumericOp::MUL ) } );
      return;
    case BinaryOperator::DIV:
      putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::NumericOp::DIV ) } );
      return;
    case BinaryOperator::MOD:
      putValInAcc(
          RuntimeValue{ ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::NumericOp::MOD ) } );
      return;
    case BinaryOperator::CONCAT:
      putValInAcc( RuntimeValue{ ValueEvaluator::evaluateArrOp( l_operand, r_operand, ArrayBinaryOperator::CONCAT ) } );
      return;
    case BinaryOperator::DIFF:
      putValInAcc( RuntimeValue{ ValueEvaluator::evaluateArrOp( l_operand, r_operand, ArrayBinaryOperator::DIFF ) } );
      return;
    case BinaryOperator::ACCESS:
      putValInAcc( ValueEvaluator::evaluateArrAccess( left_operand_rt_val, right_operand_rt_val ) );
      return;
  }
}

void Interpreter::visit( const UnaryExprNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  node.getOperand()->accept( *this );
  RuntimeValue operand_rt_val = getRecentValFromAcc();
  if ( !operand_rt_val ) {
    throw VoidValueException( node.getPosition(), "cannot perform unary op on void" );
  }
  Value operand_val = operand_rt_val.extractValue();
  putValInAcc( RuntimeValue{ ValueEvaluator::evaluateUnaryOp( operand_val, node.getOperator() ) } );
}

void Interpreter::visit( const CastExprNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  node.getExpression()->accept( *this );
  RuntimeValue operand_rt_val = getRecentValFromAcc();
  if ( !operand_rt_val ) {
    throw VoidValueException( node.getPosition(), "cannot cast void" );
  }
  Value operand_val = operand_rt_val.extractValue();
  putValInAcc( RuntimeValue{ ValueEvaluator::evaluateCastOp( operand_val, node.getType() ) } );
}

void Interpreter::visit( const ArrayIdentifierOpNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  node.getExpression().accept( *this );
  RuntimeValue runtime_val = getRecentValFromAcc();
  if ( !std::holds_alternative<ArrayType>( runtime_val.getType().internal_ ) ) {
    throw NotAllowedTypeException( node.getPosition(), "array identifier op's are only available on array types" );
  }
  Value val_inside = runtime_val.extractValue();
  auto& arr_inside = std::get<Value::ArrayValue>( val_inside.getData() );

  ///@TODO check signature maybe
  if ( arr_inside.empty() ) {
    putValInAcc( RuntimeValue{ Value{ Value::ArrayValue{} } } );
    return;
  }
  std::vector<RuntimeValue> mock_call_args;
  mock_call_args.push_back( RuntimeValue{ arr_inside[0].copy() } );  // arg is fist elem of arr
  Value::ArrayValue new_arr;
  new_arr.reserve( arr_inside.size() );

  auto func = environment_.getFunctionBySignature( node.getIdentifier(), mock_call_args );
  if ( !func ) {
    throw InvalidAccessException( node.getPosition(), "no compatible function found" );
  }
  auto& actual_func = func.value().get();

  switch ( node.getType() ) {
    case ArrayIdentifierOpType::FILTER: {  // arr<T> -> bool foo(T) -> arr<T>/c

      if ( actual_func.getReturnType() != BaseType::BOOL ) {
        throw NotAllowedTypeException(
            node.getPosition(),
            "operator filter requires function to return type 'bool' (note: signature 'bool foo(T)')" );
      }
      for ( const auto& arg : arr_inside ) {
        putValInAcc( RuntimeValue{ arg.copy() } );
        actual_func.accept( *this );
        if ( getRecentValFromAcc().evaluateValue<bool>() ) {
          new_arr.push_back( arg.copy() );
        }
      }
    }
    case ArrayIdentifierOpType::MAP: {
      for ( const auto& arg : arr_inside ) {
        putValInAcc( RuntimeValue{ arg.copy() } );
        actual_func.accept( *this );
        new_arr.push_back( getRecentValFromAcc().extractValue() );
      }
    }
  }
  putValInAcc( RuntimeValue{ std::move( new_arr ) } );
}

void Interpreter::visit( const FunctionCallNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  std::vector<RuntimeValue> call_args;
  call_args.reserve( node.getArguments().size() );
  for ( const auto& call_arg_decl : node.getArguments() ) {
    call_arg_decl->accept( *this );
    RuntimeValue arg_val = getRecentValFromAcc();
    if ( !arg_val ) {
      throw VoidValueException( call_arg_decl->getPosition(), "'void' is not a valid call arg" );
    }
    call_args.push_back( std::move( arg_val ) );
  }
  auto fun = environment_.getFunctionBySignature( node.getIdentifier(), call_args );
  if ( !fun ) {
    throw UnknownIdentifierException( node.getPosition(), "no matching function found" );
  }
  fun.value().get().accept( *this );
  // std::visit( Overloaded{ [&]( std::reference_wrapper<const FunctionDefNode> fun_def_node ) {
  //                          for ( auto& call_arg_val : call_args ) {
  //                            putValInAcc( std::move( call_arg_val ) );
  //                          }
  //                          fun_def_node.get().accept( *this );
  //                        },
  //                         [&]( const BuiltinFunction& b_fun ) {
  //                           std::vector<Value> call_args_eval;
  //                           call_args_eval.reserve( call_args.size() );
  //                           for ( auto& call_arg : call_args ) {
  //                             call_arg.extractValue();
  //                           }
  //                           b_fun.getMappedFunction()( call_args_eval );
  //                         } },
  //             fun->get() );
  // visiting/calling function already puts ret val in acc
}

void Interpreter::visit( const ArrayLiteralNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  // assert( node.getPositions().size() > 0 && "empty literal is illegal [parser]" );
  if ( node.getPositions().empty() ) {
    // throw VoidValueException( node.getPositions()[0]->getPosition(), "array literal position cannot be void" );
    putValInAcc( RuntimeValue{ Value::makeArray() } );
    return;
  }
  std::vector<Value> array_positions;
  array_positions.reserve( node.getPositions().size() );
  node.getPositions()[0]->accept( *this );
  RuntimeValue first_val = getRecentValFromAcc();

  const Type& deduced_type = first_val.getType();
  array_positions.push_back( first_val.extractValue() );

  for ( auto i = 1u; i < node.getPositions().size(); ++i ) {
    const auto& initializer_expr = node.getPositions()[i];
    initializer_expr->accept( *this );
    RuntimeValue nth_value = getRecentValFromAcc();
    if ( !nth_value ) {
      throw VoidValueException( initializer_expr->getPosition(), "array literal n cannot be void" );
    }
    if ( nth_value.getType() != deduced_type ) {
      throw NotAllowedTypeException( initializer_expr->getPosition(), "array must be homogenous" );
    }
    array_positions.push_back( nth_value.extractValue() );
  }
  Value composed_array{ std::move( array_positions ) };
  putValInAcc( RuntimeValue{ std::move( composed_array ) } );
}

void Interpreter::visit( const LiteralExprNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  putValInAcc( RuntimeValue{ node.getValue().copy() } );
}

void Interpreter::visit( const PrimaryIdentifierNode& node ) {
  DebugGuard dbg_guard{ *this, node };

  auto var = environment_.getVarByName( node.getIdentifier() );
  if ( !var ) {
    throw UnknownIdentifierException( node.getPosition(), "usage of unknown variable" );
  }
  putValInAcc( RuntimeValue{ var->get() } );
}

void Interpreter::visit( const ProgramNode& node ) {
  {
    DebugGuard dbg_guard{ *this, node };

    for ( const auto& func_def_ptr : node.getFunctionList() ) {
      if ( !environment_.tryAddUserFunction( *func_def_ptr ) ) {
        throw InvalidOverloadException( node.getPosition(), "duplicated function signature" );
      }
    }
    handleStatementList( node.getStatementList(), CallContext::ContextType::TOP_LEVEL );
  }
  // throw std::runtime_error( "check if call stack and acc empty after program execution" );
  //@TODO maybe check if call stack is empty
  assert( accumulator_.empty() && "acc should be empty after program ends" );
}

void Interpreter::visit( const BuiltinFunction& node ) {
  auto f = node.getMappedFunction();
  std::vector<RuntimeValue> call_args;
  for ( const auto& _ : node.getParameters() ) {
    call_args.push_back( getRecentValFromAcc() );
  }
}
