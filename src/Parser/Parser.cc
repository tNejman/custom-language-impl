#include "Parser/Parser.h"

#include <stdexcept>

#include "Parser/Node.hpp"
#include "Parser/Types.hpp"

void Parser::nextToken() {
  this->current_token_ = lexer_.getNextToken();
}

void Parser::skipComments() {
  while ( this->current_token_.type_ == TokenType::COMMENT ) nextToken();
}

std::unique_ptr<INode> Parser::tryBuildStatement() {
  nextToken();
  skipComments();
  switch ( current_token_.type_ ) {
    case TokenType::END_OF_FILE: return nullptr;
    case TokenType::KW_DEF: return tryBuildFunctionDef();
    case TokenType::KW_VAR: return tryBuildVariableDecl( true );
    case TokenType::T_INT:
    case TokenType::T_FLOAT:
    case TokenType::T_CHAR:
    case TokenType::T_STR:
    case TokenType::T_BOOL: return tryBuildVariableDecl( false );
    case TokenType::KW_IF: return tryBuildIfStmt();
    case TokenType::KW_WHILE: return tryBuildWhileStmt();
    case TokenType::KW_BREAK:
    case TokenType::KW_CONTINUE: return tryBuildControlFlowStmt();
    case TokenType::KW_RET: return tryBuildReturnStmt();
    default: return tryBuildExpressionStmt();
  }
}

std::unique_ptr<INode> Parser::tryBuildFunctionDef() {
  //   std::unique_ptr<INode> function_node = std::make_unique<FunctionDefNode>();
  nextToken();  // "def"
  auto return_type = tryBuildReturnType();
  nextToken();
  if ( current_token_.type_ != TokenType::IDENTIFIER ) throw std::runtime_error( "" );
  auto identifier = current_token_;
  nextToken();
  if ( current_token_.type_ != TokenType::LPAREN ) throw std::runtime_error( "" );
  nextToken();
  auto param_list = tryBuildParamList();
  nextToken();
  if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
  nextToken();
  auto block = tryBuildBlock();

  return std::make_unique<FunctionDefNode>( identifier.position_, std::get<std::string>( identifier.value_ ),
                                            std::move( return_type ), std::move( param_list ), std::move( block ) );
}

std::unique_ptr<Type> Parser::tryBuildReturnType() {
  if ( current_token_.type_ == TokenType::T_VOID ) return std::make_unique<Type>( BaseType::VOID );
  auto type_ptr = tryBuildType();
}

std::unique_ptr<INode> Parser::buildVariableDecl( const bool is_mutable ) {
  std::unique_ptr<INode> variable_decl_node = std::make_unique<VariableDeclNode>();
  if ( !is_mutable ) nextToken();
  // type
  nextToken();
  // identifier
  nextToken();
  // op assign
  nextToken();
  // expression
  return variable_decl_node;
}

std::unique_ptr<INode> Parser::buildIfStmt(){ std::unique_ptr<INode> variable_decl_node }

std::unique_ptr<Type> Parser::tryBuildType() {
  switch ( current_token_.type_ ) {
    case TokenType::T_BOOL: return std::make_unique<Type>( BaseType::BOOL );
    case TokenType::T_CHAR: return std::make_unique<Type>( BaseType::CHAR );
    case TokenType::T_FLOAT: return std::make_unique<Type>( BaseType::FLOAT );
    case TokenType::T_INT: return std::make_unique<Type>( BaseType::INT );
    case TokenType::LBRACKET: {
      nextToken();
      auto underlying_type = tryBuildType();
      if ( !underlying_type ) throw std::runtime_error( "" );
      nextToken();
      if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
      return std::make_unique<Type>( ArrayType{ std::move( underlying_type ) } );
    }
    default: return nullptr;
  }
}

Program Parser::buildProgram() {
  std::vector<std::unique_ptr<INode>> statement_list;
  auto statement = tryBuildStatement();
  if ( !statement ) return Program{};

  statement_list.push_back( ( std::move( statement ) ) );

  while ( current_token_.type_ == TokenType::NEWLINE ) {
    statement = tryBuildStatement();
    if ( statement ) statement_list.push_back( std::move( statement ) );
  }
  return Program{ std::move( statement_list ) };
};
