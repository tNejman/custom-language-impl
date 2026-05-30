#include "Interpreter/Interpreter.h"

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

void CallContext::addVariable( Variable variable ) noexcept {
  variables_.push_back( std::move( variable ) );
}

const std::vector<Variable>& CallContext::getVariables() const noexcept {
  return variables_;
}

std::vector<Variable>& CallContext::getVariables() noexcept {
  return variables_;
}

Interpreter::Environment::Environment( std::unique_ptr<const ProgramNode> program,
                                       std::vector<FunctionSymbol> functions, std::stack<CallContext> call_stack )
    : program_( std::move( program ) ), functions_( std::move( functions ) ), call_stack_() {
}

namespace interpreter_helper {
template <class... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
Overloaded( Ts... ) -> Overloaded<Ts...>;
}  // namespace interpreter_helper

const FunctionSymbol* Interpreter::Environment::getFunctionByIdentifier(
    const const std::string_view identifier ) const noexcept {
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
  auto it = std::find_if( call_stack_.top().getVariables().begin(), call_stack_.top().getVariables().end(),
                          [&]( const Variable& variable ) { return variable.getIdentifier() == identifier; } );

  if ( it == call_stack_.top().getVariables().end() ) {
    return nullptr;
  }
  return &*it;
}

Value Interpreter::Environment::getRecentValFromAcc() noexcept {
  Value value_popped = std::move( accumulator_.top() );
  accumulator_.pop();
  return value_popped;
}

Interpreter::Interpreter( std::unique_ptr<const ProgramNode> program, std::vector<FunctionSymbol> functions,
                          std::stack<CallContext> call_stack )
    : environment_( std::move( program ), std::move( functions ), std::move( call_stack ) ) {
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
}

void Interpreter::visit( const VarOrConstDeclNode& node ) {
  node.getInitializerExpr()->accept( *this );
  if ( !environment_.tryAddVarOrConst( Variable{ std::string( node.getIdentifier() ), node.getType().copy(),
                                                 node.getMutability(), environment_.getRecentValFromAcc() } ) ) {
    throw std::runtime_error( "duplicated variable at current scope" );
  };
}

void Interpreter::visit( const IfStatementNode& node ) {
  bool any_block_entered = false;
  for ( const auto& [cond, block] : node.getCondBlockPairs() ) {
    cond->accept( *this );  // evaluate cond
    if ( std::get<bool>( environment_.getRecentValFromAcc().getData() ) ) {
      environment_.addCallContext( CallContext::ContextType::IF_BLOCK );
      any_block_entered = true;
      for ( const auto& stmt : block ) {
        stmt->accept( *this );
      }
      break;
    }
  }
  if ( !any_block_entered ) {
    environment_.addCallContext( CallContext::ContextType::IF_BLOCK );
    for ( const auto& stmt : node.getElseBlock() ) {
      stmt->accept( *this );
    }
  }
  environment_.popLastCallContext();
}

void Interpreter::visit( const WhileStatementNode& node ) {
  node.getCondition()->accept( *this );
  while ( std::get<bool>( environment_.getRecentValFromAcc().getData() ) ) {
    environment_.addCallContext( CallContext::ContextType::WHILE_BLOCK );
    for ( const auto& stmt : node.getBlock() ) {
      stmt->accept( *this );
    }
    environment_.popLastCallContext();
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
  /* @TODO
  1. evaluate L operand;
  2. check for rvalue (error)
  3. check for assignability
  4. evaluate R operand
  5. check for type compatibility
  6. assign
  */
}

void Interpreter::visit( const BinaryExprNode& node ) {
  node.getLeftOperand()->accept( *this );
  Value left_operand_eval = environment_.getRecentValFromAcc();
  node.getRightOperand()->accept( *this );
  Value right_operand_eval = environment_.getRecentValFromAcc();

  if ( left_operand_eval.getValueType() != right_operand_eval.getValueType() ) {
    throw std::runtime_error( "type mismatch in binary expr" );
  }

  switch ( node.getOperator() ) {
    case BinaryOperator::OR: {
      if ( left_operand_eval.getValueType() != BaseType::BOOL || right_operand_eval.getValueType() != BaseType::BOOL ) {
        throw std::runtime_error( "operator 'OR' only available for type bool" );
      }
      environment_.putValInAcc(
          Value{ std::get<bool>( left_operand_eval.getData() ) || std::get<bool>( right_operand_eval.getData() ) } );
      break;
    }
    case BinaryOperator::AND: {
      if ( left_operand_eval.getValueType() != BaseType::BOOL || right_operand_eval.getValueType() != BaseType::BOOL ) {
        throw std::runtime_error( "operator 'AND' only available for type bool" );
      }
      environment_.putValInAcc( std::get<bool>( left_operand_eval.getData() )
                                && std::get<bool>( right_operand_eval.getData() ) );
      break;
    }
    case BinaryOperator::EQ:
      environment_.putValInAcc( evaluateRelational( left_operand_eval, right_operand_eval, std::equal_to<>{} ) );
      break;
    case BinaryOperator::NEQ:
      environment_.putValInAcc( evaluateRelational( left_operand_eval, right_operand_eval, std::not_equal_to<>{} ) );
      break;
    case BinaryOperator::LT:
      environment_.putValInAcc( evaluateRelational( left_operand_eval, right_operand_eval, std::less<>{} ) );
      break;
    case BinaryOperator::LEQ:
      environment_.putValInAcc( evaluateRelational( left_operand_eval, right_operand_eval, std::less_equal<>{} ) );
      break;
    case BinaryOperator::GT:
      environment_.putValInAcc( evaluateRelational( left_operand_eval, right_operand_eval, std::greater<>{} ) );
      break;
    case BinaryOperator::GEQ:
      environment_.putValInAcc( evaluateRelational( left_operand_eval, right_operand_eval, std::greater_equal<>{} ) );
      break;
    case BinaryOperator::ADD:
    case BinaryOperator::SUB:
    case BinaryOperator::CONCAT:
    case BinaryOperator::DIFF:
    case BinaryOperator::MUL:
    case BinaryOperator::DIV:
    case BinaryOperator::MOD:
    case BinaryOperator::FILTER:
    case BinaryOperator::MAP:
    case BinaryOperator::ACCESS:
  }
}

void Interpreter::visit( const UnaryExprNode& node ) {
}

void Interpreter::visit( const CastExprNode& node ) {
}

void Interpreter::visit( const FunctionCallNode& node ) {
}

void Interpreter::visit( const ArrayLiteralNode& node ) {
  environment_.add
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
  environment_.addCallContext( CallContext::ContextType::TOP_LEVEL );
  for ( const auto& stmt_ptr : node.getStatementList() ) {
    stmt_ptr->accept( *this );
  }
}
