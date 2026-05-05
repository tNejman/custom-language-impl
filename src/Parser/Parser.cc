#include "Parser/Parser.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>

#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.h"
#include "Parser/ParserHelper.h"
#include "Parser/Types.hpp"
#include "Parser/Variable.h"

void Parser::nextToken() {
  if ( token_buffer_ ) {
    current_token_ = std::move( *token_buffer_ );
    token_buffer_ = std::nullopt;
  } else {
    this->current_token_ = lexer_.getNextToken();
  }
}

Token Parser::peek() {
  if ( !token_buffer_ ) this->token_buffer_ = lexer_.getNextToken();
  return *token_buffer_;
}

void Parser::skipComments() {
  while ( this->current_token_.type_ == TokenType::COMMENT ) nextToken();
}

std::vector<std::unique_ptr<INode>> Parser::tryBuildStatementList() {
  std::vector<std::unique_ptr<INode>> statement_list;
  auto statement = tryBuildStatement();
  if ( !statement ) return {};

  statement_list.push_back( ( std::move( statement ) ) );

  while ( current_token_.type_ == TokenType::NEWLINE ) {
    statement = tryBuildStatement();
    if ( statement ) statement_list.push_back( std::move( statement ) );
  }
  return std::move( statement_list );
}

std::unique_ptr<INode> Parser::tryBuildStatement() {
  nextToken();
  skipComments();
  switch ( current_token_.type_ ) {
    case TokenType::END_OF_FILE: return nullptr;
    case TokenType::KW_DEF: return tryBuildFunctionDef();
    case TokenType::KW_VAR: return tryBuildVariableDecl( Mutability::MUTABLE );
    case TokenType::T_INT:
    case TokenType::T_FLOAT:
    case TokenType::T_CHAR:
    case TokenType::T_STR:
    case TokenType::T_BOOL: return tryBuildVariableDecl( Mutability::IMMUTABLE );
    case TokenType::KW_IF: return tryBuildIfStmt();
    case TokenType::KW_WHILE: return tryBuildWhileStmt();
    case TokenType::KW_BREAK:
    case TokenType::KW_CONTINUE: return tryBuildControlFlowStmt();
    case TokenType::KW_RET: return tryBuildReturnStmt();
    default: return tryBuildExpression();
  }
}

std::unique_ptr<INode> Parser::tryBuildFunctionDef() {
  nextToken();  // "def"
  Type return_type = tryBuildReturnType();
  nextToken();
  if ( current_token_.type_ != TokenType::IDENTIFIER ) throw std::runtime_error( "" );
  Token identifier_token = current_token_;
  nextToken();
  if ( current_token_.type_ != TokenType::LPAREN ) throw std::runtime_error( "" );
  nextToken();
  std::vector<std::unique_ptr<ParameterDecl>> param_list = tryBuildParamList();
  nextToken();
  if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
  nextToken();
  Block block = tryBuildBlock();

  return std::make_unique<FunctionDefNode>( identifier_token.position_,
                                            std::get<std::string>( std::move( identifier_token.value_ ) ),
                                            std::move( return_type ), std::move( param_list ), std::move( block ) );
}

Type Parser::tryBuildReturnType() {
  if ( current_token_.type_ == TokenType::T_VOID ) return Type{ BaseType::VOID };
  auto type_ptr = tryBuildType();
  if ( !type_ptr ) throw std::runtime_error( "" );
  return std::move( *type_ptr );
}

std::vector<std::unique_ptr<ParameterDecl>> Parser::tryBuildParamList() {
  std::unique_ptr<ParameterDecl> param = tryBuildParameter();
  if ( !param ) return {};
  std::vector<std::unique_ptr<ParameterDecl>> params{};
  params.push_back( std::move( param ) );

  while ( peek().type_ == TokenType::COMMA ) {
    param = tryBuildParameter();
    if ( !param ) throw std::runtime_error( "" );
    params.push_back( ( std::move( param ) ) );
  }
  return std::move( params );
}

std::unique_ptr<ParameterDecl> Parser::tryBuildParameter() {
  Mutability mut = Mutability::IMMUTABLE;
  PassMode pass_m = PassMode::REFERENCE;
  nextToken();
  bool found_quantifier = false;
  if ( current_token_.type_ == TokenType::KW_COPY ) {
    pass_m = PassMode::COPY;
    found_quantifier = true;
    nextToken();
  } else if ( current_token_.type_ == TokenType::KW_MUT ) {
    mut = Mutability::MUTABLE;
    found_quantifier = true;
    nextToken();
  }
  std::optional<Type> type = tryBuildType();
  if ( found_quantifier && !type )
    throw std::runtime_error( "" );
  else if ( !found_quantifier && !type )
    return nullptr;

  nextToken();
  if ( current_token_.type_ != TokenType::IDENTIFIER ) throw std::runtime_error( "" );

  return std::make_unique<ParameterDecl>( std::get<std::string>( std::move( current_token_.value_ ) ),
                                          std::move( *type ), pass_m, mut );
}

Block Parser::tryBuildBlock() {
  if ( current_token_.type_ != TokenType::KW_DO ) throw std::runtime_error( "" );
  nextToken();
  Block statement_list = tryBuildStatementList();
  nextToken();
  if ( current_token_.type_ != TokenType::KW_DONE ) throw std::runtime_error( "" );
  return std::move( statement_list );
}

std::unique_ptr<INode> Parser::tryBuildVariableDecl( const Mutability mutability ) {
  if ( mutability == Mutability::MUTABLE ) {
    nextToken();  // "var"
  }
  auto type = tryBuildType();
  nextToken();
  if ( current_token_.type_ != TokenType::IDENTIFIER ) throw std::runtime_error( "" );
  Token identifier_token = current_token_;
  nextToken();
  if ( current_token_.type_ != TokenType::OP_ASSIGN ) throw std::runtime_error( "" );
  std::unique_ptr<IExpressionNode> expr = tryBuildExpression();
  if ( !expr ) throw std::runtime_error( "" );

  return std::make_unique<VarOrConstDeclNode>( identifier_token.position_,
                                               std::get<std::string>( std::move( identifier_token.value_ ) ),
                                               mutability, std::move( *type ), std::move( expr ) );
}

std::unique_ptr<INode> Parser::tryBuildIfStmt() {
  std::vector<std::pair<std::unique_ptr<IExpressionNode>, Block>> condition_block_pairs;
  condition_block_pairs.push_back( tryBuildParenthesizedExpressionAndBlock() );
  while ( peek().type_ == TokenType::KW_ELSEIF ) {
    nextToken();
    auto cond_do_pair = tryBuildParenthesizedExpressionAndBlock();
    condition_block_pairs.push_back( std::move( cond_do_pair ) );
  }
  Block else_block;
  if ( peek().type_ == TokenType::KW_ELSE ) {
    nextToken();
    else_block = tryBuildBlock();
  }
  return std::make_unique<IfStatementNode>( std::move( condition_block_pairs ), std::move( else_block ) );
}

std::pair<std::unique_ptr<IExpressionNode>, Block> Parser::tryBuildParenthesizedExpressionAndBlock() {
  nextToken();
  if ( current_token_.type_ != TokenType::LPAREN ) throw std::runtime_error( "" );
  nextToken();
  auto expr = tryBuildExpression();
  if ( !expr ) throw std::runtime_error( "" );
  nextToken();
  if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
  nextToken();
  Block block = tryBuildBlock();
  return std::pair<std::unique_ptr<IExpressionNode>, Block>( std::move( expr ), std::move( block ) );
};

std::unique_ptr<INode> Parser::tryBuildWhileStmt() {
  nextToken();  // "while"
  auto expr_block_pair = tryBuildParenthesizedExpressionAndBlock();
  return std::make_unique<WhileStatementNode>( std::move( expr_block_pair.first ),
                                               std::move( expr_block_pair.second ) );
}

std::unique_ptr<INode> Parser::tryBuildControlFlowStmt() {
  ControlFlowType type =
      current_token_.type_ == TokenType::KW_BREAK ? ControlFlowType::BREAK : ControlFlowType::CONTINUE;
  return std::make_unique<ControlFlowNode>( type );
}

std::unique_ptr<INode> Parser::tryBuildReturnStmt() {
  auto expr = tryBuildExpression();
  return std::make_unique<ReturnNode>( std::move( expr ) );
}

std::optional<Type> Parser::tryBuildType() {
  switch ( current_token_.type_ ) {
    case TokenType::T_BOOL: return Type{ BaseType::BOOL };
    case TokenType::T_CHAR: return Type{ BaseType::CHAR };
    case TokenType::T_FLOAT: return Type{ BaseType::FLOAT };
    case TokenType::T_INT: return Type{ BaseType::INT };
    case TokenType::LBRACKET: {
      nextToken();
      auto underlying_type = tryBuildType();
      if ( !underlying_type ) throw std::runtime_error( "" );
      nextToken();
      if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
      return Type{ ArrayType{ std::make_unique<Type>( std::move( *underlying_type ) ) } };
    }
    default: return std::nullopt;
  }
}

std::unique_ptr<IExpressionNode> Parser::tryBuildExpression() {
  auto left_node = tryBuildLogicalOrExpr();
  if ( !left_node ) return nullptr;
  if ( !parser_helper::isAssignment( peek().type_ ) ) return left_node;

  nextToken();
  Token assignemt_token = current_token_;
  auto right_node = tryBuildExpression();
  if ( !right_node ) throw std::runtime_error( "no right node after op assign" );

  return std::make_unique<AssignmentExprNode>(
      std::move( left_node ), std::move( right_node ),
      parser_helper::translateTokenTypeToAssignmentType( assignemt_token.type_ ) );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildLogicalOrExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildLogicalAndExpr,
                             [&]( TokenType token_type ) { return token_type == TokenType::OP_OR; } );
};

std::unique_ptr<IExpressionNode> Parser::tryBuildLogicalAndExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildEqualityExpr,
                             [&]( TokenType token_type ) { return token_type == TokenType::OP_AND; } );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildEqualityExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildRelationalExpr, &parser_helper::isEquality );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildRelationalExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildAdditiveExpr, &parser_helper::isRelational );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildAdditiveExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildMultiplicativeExpr, &parser_helper::isAdditive );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildMultiplicativeExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildUnaryExpr, &parser_helper::isMultiplicative );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildUnaryExpr() {
  if ( parser_helper::isUnary( current_token_.type_ ) ) {
    Token operator_token = current_token_;
    nextToken();
    auto operand = tryBuildUnaryExpr();
    if ( !operand ) throw std::runtime_error( "" );
    return std::make_unique<UnaryExprNode>( std::move( operand ),
                                            parser_helper::translateTokenTypeToUnaryOperator( operator_token.type_ ) );
  }

  return tryBuildCastExpr();
};

std::unique_ptr<IExpressionNode> Parser::tryBuildCastExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildAccessExpr, [&]( TokenType tt ) { return tt == TokenType::KW_CAST_TO; } );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildAccessExpr() {
  auto left_node = tryBuildPrimaryExpr();
  if ( !left_node ) return nullptr;
  nextToken();
  Token operator_token = current_token_;
  while ( parser_helper::isAccesExprSufBeg( peek().type_ ) ) {
    nextToken();
    std::unique_ptr<IExpressionNode> right_node = nullptr;
    switch ( current_token_.type_ ) {
      case TokenType::LBRACKET: {
        nextToken();
        right_node = tryBuildExpression();
        if ( !right_node ) throw std::runtime_error( "" );
        nextToken();
        if ( current_token_.type_ != TokenType::RBRACKET ) throw std::runtime_error( "" );
        left_node =
            std::make_unique<BinaryExprNode>( std::move( left_node ), std::move( right_node ), BinaryOperator::ACCESS );
        break;
      }
      case TokenType::OP_FILTER:
      case TokenType::OP_MAP: {
        nextToken();
        if ( current_token_.type_ != TokenType::IDENTIFIER ) throw std::runtime_error( "" );
        left_node = std::make_unique<BinaryExprNode>(
            std::move( left_node ),
            std::make_unique<PrimaryIdentifierNode>( std::get<std::string>( std::move( current_token_.value_ ) ) ),
            parser_helper::translateTokenTypeToBinaryOperator( operator_token.type_ ) );
      }
    }
  }
  return left_node;
}

std::unique_ptr<IExpressionNode> Parser::tryBuildPrimaryExpr() {
  if ( current_token_.type_ == TokenType::IDENTIFIER ) {
    Token identifier_token = current_token_;
    if ( peek().type_ != TokenType::LPAREN ) {
      return std::make_unique<PrimaryIdentifierNode>( std::get<std::string>( std::move( identifier_token.value_ ) ) );
    }
    nextToken();
    auto arguments = tryBuildArgumentListExpr();
    nextToken();
    if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
    return std::make_unique<FunctionCallNode>( std::get<std::string>( std::move( identifier_token.value_ ) ),
                                               std::move( arguments ) );
  }
  if ( current_token_.type_ == TokenType::LPAREN ) {
    auto expr = tryBuildExpression();
    nextToken();
    if ( current_token_.type_ != TokenType::RPAREN ) throw std::runtime_error( "" );
  }
  if ( parser_helper::isLiteral( current_token_.type_ ) ) {
    return tryBuildLiteralExpr();
  }
  return tryBuildArrayLiteralExpr();
}

std::vector<std::unique_ptr<IExpressionNode>> Parser::tryBuildArgumentListExpr() {
  auto expr = tryBuildExpression();
  if ( !expr ) return {};
  std::vector<std::unique_ptr<IExpressionNode>> argument_list;
  argument_list.push_back( std::move( expr ) );

  while ( peek().type_ == TokenType::COMMA ) {
    nextToken();
    auto expr = tryBuildExpression();
    if ( !expr ) throw std::runtime_error( "" );
    argument_list.push_back( std::move( expr ) );
  }
  return std::move( argument_list );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildArrayLiteralExpr() {
  if ( current_token_.type_ != TokenType::LBRACKET ) throw std::runtime_error( "" );
  auto array_positions = tryBuildArgumentListExpr();
  nextToken();
  if ( current_token_.type_ != TokenType::RBRACKET ) throw std::runtime_error( "" );
  return std::make_unique<ArrayLiteralNode>( std::move( array_positions ) );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildLiteralExpr() {
  if ( !parser_helper::isLiteral( current_token_.type_ ) ) {
    return nullptr;
  }
  switch ( current_token_.type_ ) {
    case TokenType::BOOL_LITERAL:
      return std::make_unique<LiteralExprNode>( Type{ BaseType::BOOL },
                                                Value{ std::get<bool>( current_token_.value_ ) } );
    case TokenType::CHAR_LITERAL:
      return std::make_unique<LiteralExprNode>( Type{ BaseType::CHAR },
                                                Value{ std::get<char>( current_token_.value_ ) } );
    case TokenType::FLOAT_LITERAL:
      return std::make_unique<LiteralExprNode>( Type{ BaseType::FLOAT },
                                                Value{ std::get<float>( current_token_.value_ ) } );
    case TokenType::INT_LITERAL:
      return std::make_unique<LiteralExprNode>( Type{ BaseType::INT },
                                                Value{ std::get<int>( current_token_.value_ ) } );
    case TokenType::STRING_LITERAL: {
      std::string str_lit = std::get<std::string>( current_token_.value_ );
      return std::make_unique<LiteralExprNode>( Type{ ArrayType{ std::make_unique<Type>( BaseType::CHAR ) } },
                                                Value{ std::vector<Value>( str_lit.begin(), str_lit.end() ) } );
    }
  }
}

Program Parser::buildProgram() {
  std::vector<std::unique_ptr<INode>> statement_list = tryBuildStatementList();
  return Program{ std::move( statement_list ) };
};
