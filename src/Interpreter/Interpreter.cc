#include "Interpreter/Interpreter.h"

#include <algorithm>
#include <stdexcept>

#include "Interpreter/Variable.h"

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
}

void Interpreter::visit( const WhileStatementNode& node ) {
}

void Interpreter::visit( const ControlFlowNode& node ) {
}

void Interpreter::visit( const ReturnNode& node ) {
}

void Interpreter::visit( const AssignmentExprNode& node ) {
}

void Interpreter::visit( const BinaryExprNode& node ) {
}

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
  for ( const auto& stmt_ptr : node.getStatementList() ) {
    stmt_ptr->accept( *this );
  }
}
