#include "Interpreter/Interpreter.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include "Interpreter/CallStack.h"
#include "Interpreter/InterpreterHelper.hpp"
#include "Interpreter/ValueEvaluator.hpp"
#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

/* Environment


*/

const FunctionSymbol* Interpreter::Environment::getFunctionByIdentifier(
    const std::string_view identifier ) const noexcept {
  auto it = std::ranges::find_if( functions_, [&]( const auto& function_symbol ) {
    return std::visit( interpreter_helper::Overloaded{
                           [&identifier]( const FunctionDefNode* node ) { return node->getIdentifier() == identifier; },
                           [&]( const BuiltinFunction& builtin ) { return builtin.getIdentifier() == identifier; } },
                       function_symbol );
  } );

  if ( it == functions_.end() ) return nullptr;
  return &*it;
}

const FunctionSymbol* Interpreter::Environment::getFunctionBySignature(
    const std::string_view identifier, const Type& return_type,
    const std::vector<ParameterDecl>& parameters ) const noexcept {
  if ( identifier == "write" ) {
    return getFunctionByIdentifier( "write" );
  }
  auto it = std::ranges::find_if( functions_, [&]( const auto& function_symbol ) {
    return std::visit( interpreter_helper::Overloaded{ [&]( const FunctionDefNode* node ) {
                                                        return node->getIdentifier() == identifier
                                                               && node->getType() == return_type
                                                               && node->getParameters() == parameters;
                                                      },
                                                       [&]( const BuiltinFunction& builtin ) {
                                                         return builtin.getIdentifier() == identifier
                                                                && builtin.getReturnType() == return_type
                                                                && builtin.getParameters() == parameters;
                                                       } },
                       function_symbol );
  } );

  if ( it == functions_.end() ) return nullptr;
  return &*it;
}

const FunctionSymbol* Interpreter::Environment::getFunctionBySignature( const BuiltinFunction& func ) const noexcept {
  return getFunctionBySignature( func.getIdentifier(), func.getReturnType(), func.getParameters() );
}

const FunctionSymbol* Interpreter::Environment::getFunctionBySignature( const FunctionDefNode* node ) const noexcept {
  return getFunctionBySignature( node->getIdentifier(), node->getType(), node->getParameters() );
}

Interpreter::Environment::Environment( std::unique_ptr<const ProgramNode> program )
    : program_( std::move( program ) ), functions_(), call_stack_() {
}

bool Interpreter::Environment::tryAddUserFunction( const FunctionDefNode* node ) {
  if ( node == nullptr || getFunctionBySignature( node ) ) {
    return false;
  }
  functions_.push_back( node );
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
  if ( getVarOrConstByNameCurScope( variable.getIdentifier() ) ) {
    return false;
  }
  call_stack_.top().addVariable( std::move( variable ) );
  return true;
}

const Variable* Interpreter::Environment::getVarOrConstByNameCurScope(
    const std::string_view identifier ) const noexcept {
  auto it = std::find_if( call_stack_.top().getVariables().begin(), call_stack_.top().getVariables().end(),
                          [&]( const Variable& variable ) { return variable.getIdentifier() == identifier; } );

  if ( it == call_stack_.top().getVariables().end() ) {
    return nullptr;
  }
  return &*it;
}

Variable* Interpreter::Environment::getVarOrConstByNameCurScope( const std::string_view identifier ) noexcept {
  for ( auto i = call_stack_.size() - 1; i >= 0; --i ) {  // iterating from top to bottom
    auto& call_context = call_stack_.nth( i )->get();
    auto it = std::find_if( call_context.getVariables().begin(), call_context.getVariables().end(),
                            [&]( const Variable& var ) { return var.getIdentifier() == identifier; } );
    if ( it != call_context.getVariables().end() ) {
      return &*it;
    }
    if ( call_context.getType() == CallContext::ContextType::FUNCTION_CALL ) {
      break;  // cannot view variables above function
    }
  }
  return nullptr;
}

Variable* Interpreter::Environment::getVarOrConstByNameGlobalScope( const std::string_view identifier ) noexcept {
  auto top_level_var_vect_wrap = call_stack_.nth( 0 );
  assert( top_level_var_vect_wrap && "getting variable encountered dropped top level context" );
  auto& top_level_var_vect = top_level_var_vect_wrap->get().getVariables();
  auto it = std::find_if( top_level_var_vect.begin(), top_level_var_vect.end(),
                          [&]( const Variable& var ) { return var.getIdentifier() == identifier; }

  );
  if ( it == top_level_var_vect.end() ) {
    return nullptr;
  }
  return &*it;
}

bool Interpreter::Environment::isAssignableTo( const AccumulatorVal& val ) const noexcept {
  // must be Lvalue to assign to
  return std::visit( Overloaded{ []( const Value& _ ) { return false; },
                                 []( const std::reference_wrapper<Variable> var ) {
                                   return var.get().getMutability() == Mutability::MUTABLE;
                                 } },
                     val );
}

AccumulatorVal Interpreter::Environment::getRecentValFromAcc() {
  if ( accumulator_.empty() ) {
    throw std::runtime_error( "tried fetching from empty accumulator" );
  }
  auto value_popped = std::move( accumulator_.top() );
  accumulator_.pop();
  return value_popped;
}
void Interpreter::Environment::putValInAcc( Value val ) noexcept {
  accumulator_.push( std::move( val ) );
}
void Interpreter::Environment::putValInAcc( std::reference_wrapper<Variable> var ) noexcept {
  accumulator_.push( var );
}

size_t Interpreter::Environment::getAccSize() const noexcept {
  return accumulator_.size();
}

void Interpreter::Environment::addCallContext( CallContext::ContextType context_type ) {
  call_stack_.push( CallContext{ context_type } );
}

void Interpreter::Environment::addCallContext( const FunctionDefNode* func_def ) {
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
      return it->getFunctionSig()->getType();
    }
  }
  throw std::runtime_error( "cannot retriever type; not in function" );
}

/* Interpreter
 */

void Interpreter::handleStatementList( const std::vector<std::unique_ptr<INode>>& statements,
                                       CallContext::ContextType context_type ) {
  environment_.addCallContext( context_type );
  for ( const auto& stmt_ptr : statements ) {
    const size_t initial_acc_size = environment_.getAccSize();

    stmt_ptr->accept( *this );

    /* statement may leave value in acc, like
    1+1
    at top level; must be popped to restore the acc stack orignal size
    */
    assert( environment_.getAccSize() >= initial_acc_size && "accumulator mustn't shrink after statement handle" );
    while ( environment_.getAccSize() > initial_acc_size ) {
      environment_.getRecentValFromAcc();
    }
  }
  environment_.popLastCallContext();
}

Interpreter::Interpreter( std::unique_ptr<const ProgramNode> program ) : environment_( std::move( program ) ) {
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
}

void Interpreter::visit( const FunctionDefNode& node ) {
  environment_.addCallContext( &node );
  // @TODO
}

void Interpreter::visit( const VarOrConstDeclNode& node ) {
  node.getInitializerExpr()->accept( *this );
  AccumulatorVal acc_val = environment_.getRecentValFromAcc();
  Value assigned_value =
      std::visit( Overloaded{ []( const std::reference_wrapper<Variable> var ) { return var.get().getValue().copy(); },
                              []( Value val ) { return val; } },
                  std::move( acc_val ) );

  if ( node.getType() != assigned_value.getValueType() ) {
    throw std::runtime_error( "initializer value does not match variable type" );
  }

  if ( !environment_.tryAddVarOrConst( Variable{ std::string( node.getIdentifier() ), node.getType().copy(),
                                                 node.getMutability(), std::move( assigned_value ) } ) ) {
    throw std::runtime_error( "duplicated variable at current scope" );
  };
}

void Interpreter::visit( const IfStatementNode& node ) {
  bool any_block_entered = false;
  for ( const auto& [cond, block] : node.getCondBlockPairs() ) {
    cond->accept( *this );

    bool cond_evaluated = [&]() {
      try {
        return interpreter_helper::evaluateAccVal<bool>( environment_.getRecentValFromAcc() );
      } catch ( const std::bad_variant_access& ) {
        throw std::runtime_error( "if condition must be type bool" );
      }
    }();

    if ( cond_evaluated ) {
      handleStatementList( block, CallContext::ContextType::IF_BLOCK );
      any_block_entered = true;
      break;
    }
  }
  if ( !any_block_entered ) {
    handleStatementList( node.getElseBlock(), CallContext::ContextType::IF_BLOCK );
  }
  // call context managed by 'handleStatementList'
}

void Interpreter::visit( const WhileStatementNode& node ) {
  node.getCondition()->accept( *this );
  while ( [&]() {
    try {
      return interpreter_helper::evaluateAccVal<bool>( environment_.getRecentValFromAcc() );
    } catch ( const std::bad_variant_access& ) {
      throw std::runtime_error( "while condition must be type bool" );
    }
  }() ) {
    handleStatementList( node.getBlock(), CallContext::ContextType::WHILE_BLOCK );
  }
}

void Interpreter::visit( const ControlFlowNode& node ) {
  if ( !environment_.isStateInWhileLoop() ) {
    throw std::runtime_error( "loop control not in a loop" );
  }
  /// @TODO
}

void Interpreter::visit( const ReturnNode& node ) {
  if ( !environment_.isStateInFunctionBody() ) {
    throw std::runtime_error( "return statement not in a function" );
  }
  if ( node.getExpression() == nullptr ) {
    if ( environment_.getCurrentFunctionReturnType() == BaseType::VOID ) {
      /// @TODO void function
    } else {
      throw std::runtime_error( "return value in non-void function" );
    }
  }
  node.getExpression()->accept( *this );
  Value return_value = environment_.getRecentValFromAcc();
  if ( environment_.getCurrentFunctionReturnType() != return_value.getValueType() ) {
    throw std::runtime_error( "return type doesn't match function signature" );
  }
  /// @TODO non-void function
}

void Interpreter::visit( const AssignmentExprNode& node ) {
  node.getLeftOperand()->accept( *this );
  auto left_val_from_acc = environment_.getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  auto right_val_from_acc = environment_.getRecentValFromAcc();

  if ( interpreter_helper::getAccValType( left_val_from_acc )
       != interpreter_helper::getAccValType( right_val_from_acc ) ) {
    throw std::runtime_error( "assignment operand must match type" );
  }
  if ( !environment_.isAssignableTo( left_val_from_acc ) ) {  // checks for reference_wrapper<Variable> and mutability
    throw std::runtime_error( "cannot assign to immutable value" );
  }
  // left_val is only Lvalue allowed from now on
  Variable& var_assigned_to = std::get<std::reference_wrapper<Variable>>( left_val_from_acc ).get();
  Value new_value = std::visit(
      Overloaded{ [&]( const Value& val ) { return val.copy(); },
                  [&]( const std::reference_wrapper<Variable> var ) { return var.get().getValue().copy(); } },
      right_val_from_acc );
  Value new_value_assignment_type_applied = [&]() {
    switch ( node.getAssignmentType() ) {
      case AssignmentType::ASSIGN: return std::move( new_value );
      case AssignmentType::ADD_ASSIGN:
        return ValueEvaluator::evaluateNumeric( var_assigned_to.getValue(), new_value, std::plus<>{} );
      case AssignmentType::SUB_ASSIGN:
        return ValueEvaluator::evaluateNumeric( var_assigned_to.getValue(), new_value, std::minus<>{} );
      case AssignmentType::MUL_ASSIGN:
        return ValueEvaluator::evaluateNumeric( var_assigned_to.getValue(), new_value, std::multiplies<>{} );
      case AssignmentType::DIV_ASSIGN:
        return ValueEvaluator::evaluateNumeric( var_assigned_to.getValue(), new_value, std::divides<>{} );
      case AssignmentType::MOD_ASSIGN:
        return ValueEvaluator::evaluateNumeric( var_assigned_to.getValue(), new_value, ValueEvaluator::FmodModulus{} );
    }
  }();
  var_assigned_to.setValue( std::move( new_value_assignment_type_applied ) );
}

void Interpreter::visit( const BinaryExprNode& node ) {
  node.getLeftOperand()->accept( *this );
  AccumulatorVal left_operand_eval = environment_.getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  AccumulatorVal right_operand_eval = environment_.getRecentValFromAcc();

  if ( interpreter_helper::getAccValType( left_operand_eval )
       != interpreter_helper::getAccValType( right_operand_eval ) ) {
    throw std::runtime_error( "type mismatch in binary expr" );
  }
  Value l_operand = interpreter_helper::extractAccVal( left_operand_eval );
  Value r_operand = interpreter_helper::extractAccVal( right_operand_eval );

  switch ( node.getOperator() ) {
    case BinaryOperator::AND: {
      environment_.putValInAcc( std::get<bool>( l_operand.getData() ) && std::get<bool>( r_operand.getData() ) );
      return;
    }
    case BinaryOperator::OR: {
      environment_.putValInAcc( std::get<bool>( l_operand.getData() ) || std::get<bool>( r_operand.getData() ) );
      return;
    }
    case BinaryOperator::EQ: environment_.putValInAcc( l_operand == r_operand ); return;
    case BinaryOperator::NEQ: environment_.putValInAcc( l_operand != r_operand ); return;
    case BinaryOperator::LT:
      environment_.putValInAcc( ValueEvaluator::evaluateRelational( l_operand, r_operand, std::less<>{} ) );
      return;
    case BinaryOperator::LEQ:
      environment_.putValInAcc( ValueEvaluator::evaluateRelational( l_operand, r_operand, std::less_equal<>{} ) );
      return;
    case BinaryOperator::GT:
      environment_.putValInAcc( ValueEvaluator::evaluateRelational( l_operand, r_operand, std::greater<>{} ) );
      return;
    case BinaryOperator::GEQ:
      environment_.putValInAcc( ValueEvaluator::evaluateRelational( l_operand, r_operand, std::greater_equal<>{} ) );
      return;
    case BinaryOperator::ADD:
      environment_.putValInAcc( ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::plus<>{} ) );
      return;
    case BinaryOperator::SUB:
      environment_.putValInAcc( ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::minus<>{} ) );
      return;
    case BinaryOperator::MUL:
      environment_.putValInAcc( ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::multiplies<>{} ) );
      return;
    case BinaryOperator::DIV:
      environment_.putValInAcc( ValueEvaluator::evaluateNumeric( l_operand, r_operand, std::divides<>{} ) );
      return;
    case BinaryOperator::MOD:
      environment_.putValInAcc(
          ValueEvaluator::evaluateNumeric( l_operand, r_operand, ValueEvaluator::FmodModulus{} ) );
      return;
    case BinaryOperator::CONCAT:
      environment_.putValInAcc( ValueEvaluator::evaluateArrOp( l_operand, r_operand, ArrayBinaryOperator::CONCAT ) );
      return;
    case BinaryOperator::DIFF:
      environment_.putValInAcc( ValueEvaluator::evaluateArrOp( l_operand, r_operand, ArrayBinaryOperator::DIFF ) );
      return;
    case BinaryOperator::ACCESS:
      environment_.putValInAcc(
          ValueEvaluator::evaluateArrOpSpecial( l_operand, r_operand, ArrayBinarySpecialOperator::ACCESS ) );
      return;
    case BinaryOperator::MAP:
      environment_.putValInAcc(
          ValueEvaluator::evaluateArrOpSpecial( l_operand, r_operand, ArrayBinarySpecialOperator::MAP ) );
      return;
    case BinaryOperator::FILTER:
      environment_.putValInAcc(
          ValueEvaluator::evaluateArrOpSpecial( l_operand, r_operand, ArrayBinarySpecialOperator::FILTER ) );
      return;

      // if ( tryEvaluateLogicalExpr( left_operand_eval, right_operand_eval, node.getOperator() ) ) {
      // } else if ( tryEvaluateEqualityExpr( left_operand_eval, right_operand_eval, node.getOperator() ) ) {
      // } else if ( tryEvaluateRelaitonalExpr( left_operand_eval, right_operand_eval, node.getOperator() ) ) {
      // } else if ( tryEvaluateNumericExpr( left_operand_eval, right_operand_eval, node.getOperator() ) ) {
      // } else if ( tryEvaluateSetExpr( left_operand_eval, right_operand_eval, node.getOperator() ) ) {
      // } else if ( tryEvaluateAccessExpr( left_operand_eval, right_operand_eval, node.getOperator() ) ) {
      // } else {
      //   throw std::runtime_error( "none of the operations evaluated" );
      // }
  }
  //   switch ( node.getOperator() ) {
  //     case BinaryOperator::ADD:
  //     case BinaryOperator::SUB:
  //     case BinaryOperator::CONCAT:
  //     case BinaryOperator::DIFF:
  //     case BinaryOperator::MUL:
  //     case BinaryOperator::DIV:
  //     case BinaryOperator::MOD:
  //     case BinaryOperator::FILTER:
  //     case BinaryOperator::MAP:
  //     case BinaryOperator::ACCESS:
  //   }
  // }

  void Interpreter::visit( const UnaryExprNode& node ) {
  }

  void Interpreter::visit( const CastExprNode& node ) {
  }

  void Interpreter::visit( const FunctionCallNode& node ) {
  }

  void Interpreter::visit( const ArrayLiteralNode& node ) {
  }

  void Interpreter::visit( const LiteralExprNode& node ) {
  }

  void Interpreter::visit( const PrimaryIdentifierNode& node ) {
  }

  void Interpreter::visit( const ProgramNode& node ) {
    for ( const auto& func_def_ptr : node.getFunctionList() ) {
      if ( !environment_.tryAddUserFunction( func_def_ptr.get() ) ) {
        throw std::runtime_error( "duplicated function signature" );
      }
    }
    handleStatementList( node.getStatementList(), CallContext::ContextType::TOP_LEVEL );
    assert( environment_.getCallStack().empty() && "after program ends call stack must be empty" );
  }
