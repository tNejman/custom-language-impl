#include "Parser/Parser.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>

#include "Exceptions/ParserExceptions/MissingKeywordException.hpp"
#include "Exceptions/ParserExceptions/MissingNewlineException.hpp"
#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/ParserHelper.h"
#include "Parser/Types.hpp"
#include "Parser/Variable.h"

#define CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( expected_type, var_name ) \
  auto var_name##_opt = consumeSpecificTokenOrReturnNull( expected_type );  \
  if ( !var_name##_opt ) return nullptr;                                    \
  Token var_name = *std::move( var_name##_opt );

Token Parser::getFirstToken() {
  return lexer_.getNextToken();
}

Token Parser::nextToken() {
  // if ( token_buffer_.has_value() ) {
  //   current_token_ = std::move( *token_buffer_ );
  //   token_buffer_ = std::nullopt;
  // } else {
  this->current_token_ = lexer_.getNextToken();
  // }
  skipComments();
  return current_token_;
}

// Token Parser::peek() {
//   if ( !token_buffer_ ) this->token_buffer_ = lexer_.getNextToken();
//   skipBufferComments();
//   return *token_buffer_;
// }

void Parser::skipComments() {
  while ( this->current_token_.type_ == TokenType::COMMENT ) nextToken();
}

// void Parser::skipBufferComments() {
//   while ( token_buffer_ && ( *token_buffer_ ).type_ == TokenType::COMMENT ) token_buffer_ = lexer_.getNextToken();
// }

void Parser::skipNewlines() {
  while ( current_token_.type_ == TokenType::NEWLINE ) nextToken();
}

std::vector<std::unique_ptr<INode>> Parser::tryBuildStatementList() {
  skipNewlines();  // statements in a block may start with any number of newlines, even the program itslef
  auto statement = tryBuildStatement();
  if ( !statement ) return {};

  std::vector<std::unique_ptr<INode>> statement_list;
  statement_list.push_back( ( std::move( statement ) ) );

  while ( current_token_.type_ == TokenType::NEWLINE ) {
    skipNewlines();  // may be more than one newline separating statements
    if ( auto next_statement = tryBuildStatement() ) {
      statement_list.push_back( std::move( next_statement ) );
    }
  }
  return std::move( statement_list );
}

std::unique_ptr<INode> Parser::tryBuildStatement() {
  Token switched_cur_token = current_token_;
  std::unique_ptr<INode> statement_node = nullptr;
  switch ( current_token_.type_ ) {
    case TokenType::END_OF_FILE: break;
    case TokenType::KW_DEF: statement_node = tryBuildFunctionDef(); break;
    case TokenType::KW_VAR: statement_node = tryBuildVariableDecl( Mutability::MUTABLE ); break;
    case TokenType::T_INT:
    case TokenType::T_FLOAT:
    case TokenType::T_CHAR:
    case TokenType::T_STR:
    case TokenType::T_BOOL: statement_node = tryBuildVariableDecl( Mutability::IMMUTABLE ); break;
    case TokenType::KW_IF: statement_node = tryBuildIfStmt(); break;
    case TokenType::KW_WHILE: statement_node = tryBuildWhileStmt(); break;
    case TokenType::KW_BREAK:
    case TokenType::KW_CONTINUE: statement_node = tryBuildControlFlowStmt(); break;
    case TokenType::KW_RET: statement_node = tryBuildReturnStmt(); break;
    default: statement_node = tryBuildExpression(); break;
  }
  if ( !statement_node ) {
    assert( current_token_ == switched_cur_token
            && "When 'Statement' evalutes 'ε' 'current_token_' mustn't change but it did" );
  }
  return statement_node;
}

std::unique_ptr<INode> Parser::tryBuildFunctionDef() {
  static constexpr std::string_view object_being_built_tag = "function declaration";
  CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( TokenType::KW_DEF, function_def_marker );

  auto opt_return_type = tryBuildReturnType();
  if ( !opt_return_type ) throw InvalidTypeException( function_def_marker.position_, object_being_built_tag );
  // buildType consumes the type
  Token identifier_token = current_token_;
  consumeSpecificTokenOrThrow<MissingIdentifierException>( TokenType::IDENTIFIER, object_being_built_tag );
  consumeSpecificTokenOrThrow<MissingParenthesisException>( TokenType::LPAREN, object_being_built_tag,
                                                            ParenthesisType::OPENING );
  // current_token_ is the first of paramList
  std::vector<std::unique_ptr<ParameterDecl>> param_list = tryBuildParamList();
  consumeSpecificTokenOrThrow<MissingParenthesisException>( TokenType::RPAREN, object_being_built_tag,
                                                            ParenthesisType::CLOSING );
  Block block = tryBuildBlock();
  consumeSpecificTokenOrThrow<MissingNewlineException>( TokenType::NEWLINE, TokenType::KW_DONE,
                                                        object_being_built_tag );

  return std::make_unique<FunctionDefNode>(
      function_def_marker.position_, std::get<std::string>( std::move( identifier_token.value_ ) ),
      *std::move( opt_return_type ), std::move( param_list ), std::move( block ) );
}

std::optional<Type> Parser::tryBuildReturnType() {
  if ( current_token_.type_ == TokenType::T_VOID ) {
    auto ret_type = Type{ BaseType::VOID };
    nextToken();  // consume "void"
    return ret_type;
  }
  return tryBuildType();
}

std::vector<std::unique_ptr<ParameterDecl>> Parser::tryBuildParamList() {
  Token first_param_token = current_token_;
  std::unique_ptr<ParameterDecl> param = tryBuildParameter();
  if ( !param ) return {};
  std::vector<std::unique_ptr<ParameterDecl>> params{};
  params.push_back( std::move( param ) );

  while ( current_token_.type_ == TokenType::COMMA ) {
    nextToken();  // consume comma
    Token parameter_build_marker = current_token_;
    param = tryBuildParameter();
    if ( !param ) throw InvalidParameterException( parameter_build_marker.position_ );
    params.push_back( ( std::move( param ) ) );
  }
  if ( params.size() == 0 ) {
    assert( first_param_token == current_token_
            && "When 'ParamList' evalutes 'ε' 'current_token_' mustn't change but it did" );
  }
  return std::move( params );
}

std::unique_ptr<ParameterDecl> Parser::tryBuildParameter() {
  static constexpr Mutability default_param_mutability = Mutability::IMMUTABLE;
  static constexpr PassMode default_param_pass_mode = PassMode::REFERENCE;

  Mutability param_mutability = default_param_mutability;
  PassMode param_pass_mode = default_param_pass_mode;
  if ( current_token_.type_ == TokenType::KW_COPY ) {
    param_pass_mode = PassMode::COPY;
    nextToken();
  } else if ( current_token_.type_ == TokenType::KW_MUT ) {
    param_mutability = Mutability::MUTABLE;
    nextToken();
  }
  Token build_type_marker = current_token_;
  std::optional<Type> type = tryBuildType();
  if ( !type ) {
    if ( param_mutability != default_param_mutability ) {
      throw InvalidTypeException( build_type_marker.position_, param_mutability );
    } else if ( param_pass_mode != default_param_pass_mode ) {
      throw InvalidTypeException( build_type_marker.position_, param_pass_mode );
    }
    return nullptr;
  }
  Token identifier_token = current_token_;
  consumeSpecificTokenOrThrow<MissingIdentifierException>( TokenType::IDENTIFIER,
                                                           "building a parameter after a given type" );

  return std::make_unique<ParameterDecl>( std::get<std::string>( std::move( identifier_token.value_ ) ),
                                          *std::move( type ), param_pass_mode, param_mutability );
}

Block Parser::tryBuildBlock() {
  static constexpr std::string_view object_being_built_tag = "'do ... done' block";
  consumeSpecificTokenOrThrow<MissingKeywordException>( TokenType::KW_DO, TokenType::KW_DO, object_being_built_tag );
  consumeSpecificTokenOrThrow<MissingNewlineException>( TokenType::NEWLINE, TokenType::KW_DO, object_being_built_tag );
  Block statement_list = tryBuildStatementList();
  consumeSpecificTokenOrThrow<MissingKeywordException>( TokenType::KW_DONE, TokenType::KW_DONE,
                                                        object_being_built_tag );
  // consumeSpecificTokenOrThrow<MissingNewlineException>( TokenType::NEWLINE, TokenType::KW_DONE,
  //                                                       object_being_built_tag );
  return std::move( statement_list );
}

std::unique_ptr<INode> Parser::tryBuildVariableDecl( const Mutability mutability ) {
  static constexpr std::string_view object_being_built_tag = "variable declaration";
  if ( mutability == Mutability::MUTABLE ) {
    nextToken();  // "var"
  }
  auto type = tryBuildType();
  if ( !type && mutability == Mutability::MUTABLE ) {
    throw InvalidTypeException( current_token_.position_, object_being_built_tag );
  } else if ( !type ) {
    return nullptr;
  }
  Token identifier_token = current_token_;
  consumeSpecificTokenOrThrow<MissingIdentifierException>( TokenType::IDENTIFIER, object_being_built_tag );
  consumeSpecificTokenOrThrow<MissingOperatorException>( TokenType::OP_ASSIGN, TokenType::OP_ASSIGN,
                                                         object_being_built_tag );
  Token expr_beg_marker = current_token_;
  std::unique_ptr<IExpressionNode> expr = tryBuildExpression();
  if ( !expr ) throw MissingExpressionException( expr_beg_marker.position_, object_being_built_tag );

  return std::make_unique<VarOrConstDeclNode>( identifier_token.position_,
                                               std::get<std::string>( std::move( identifier_token.value_ ) ),
                                               mutability, std::move( *type ), std::move( expr ) );
}

std::unique_ptr<INode> Parser::tryBuildIfStmt() {
  CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( TokenType::KW_IF, if_begining_marker )
  ExprBlockPairVec condition_block_pairs;
  Token expr_beg_marker = current_token_;
  auto expr_and_block = tryBuildParenthesizedExpressionAndBlock();
  if ( !expr_and_block.first ) {
    throw MissingExpressionException( expr_beg_marker.position_, "if statement" );
  }
  condition_block_pairs.push_back( std::move( expr_and_block ) );
  while ( current_token_.type_ == TokenType::KW_ELSEIF ) {
    nextToken();
    Token sec_expr_beg_marker = current_token_;
    auto sec_expr_and_block = tryBuildParenthesizedExpressionAndBlock();
    if ( !sec_expr_and_block.first ) {
      throw MissingExpressionException( sec_expr_beg_marker.position_, "else if statement" );
    }
    condition_block_pairs.push_back( std::move( sec_expr_and_block ) );
  }
  Block else_block;
  if ( current_token_.type_ == TokenType::KW_ELSE ) {
    nextToken();  // consume "if"
    else_block = tryBuildBlock();
  }
  consumeSpecificTokenOrThrow<MissingNewlineException>( TokenType::NEWLINE, TokenType::KW_DONE, "if statement" );
  return std::make_unique<IfStatementNode>( if_begining_marker.position_, std::move( condition_block_pairs ),
                                            std::move( else_block ) );
}

std::pair<std::unique_ptr<IExpressionNode>, Block> Parser::tryBuildParenthesizedExpressionAndBlock() {
  static constexpr std::string_view object_being_built_tag =
      "parenthesized expression and block, i.e. '(...) do ... done'";
  // No nullptr ret because expr-block pair is cannot be ε
  consumeSpecificTokenOrThrow<MissingParenthesisException>( TokenType::LPAREN, object_being_built_tag,
                                                            ParenthesisType::OPENING );
  Token expr_beg_marker = current_token_;
  auto expr = tryBuildExpression();
  if ( !expr ) throw MissingExpressionException( expr_beg_marker.position_, object_being_built_tag );
  consumeSpecificTokenOrThrow<MissingParenthesisException>( TokenType::RPAREN, object_being_built_tag,
                                                            ParenthesisType::CLOSING );
  Block block = tryBuildBlock();
  return std::pair<std::unique_ptr<IExpressionNode>, Block>( std::move( expr ), std::move( block ) );
};

std::unique_ptr<INode> Parser::tryBuildWhileStmt() {
  CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( TokenType::KW_WHILE, while_begining_marker )
  auto expr_block_pair = tryBuildParenthesizedExpressionAndBlock();
  if ( !expr_block_pair.first ) {
    throw MissingExpressionException( current_token_.position_, "while statement" );
  }
  consumeSpecificTokenOrThrow<MissingNewlineException>( TokenType::NEWLINE, TokenType::KW_DONE, "while statement" );
  return std::make_unique<WhileStatementNode>( while_begining_marker.position_, std::move( expr_block_pair.first ),
                                               std::move( expr_block_pair.second ) );
}

std::unique_ptr<INode> Parser::tryBuildControlFlowStmt() {
  ControlFlowType control_flow_type;
  if ( current_token_.type_ == TokenType::KW_BREAK ) {
    control_flow_type = ControlFlowType::BREAK;
    nextToken();  // consume "break"
  } else if ( current_token_.type_ == TokenType::KW_CONTINUE ) {
    control_flow_type = ControlFlowType::CONTINUE;
    nextToken();  // consume "continue"
  } else {
    return nullptr;
  }
  return std::make_unique<ControlFlowNode>( current_token_.position_, control_flow_type );
}

std::unique_ptr<INode> Parser::tryBuildReturnStmt() {
  CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( TokenType::KW_RET, return_marker )
  // expr is nullable, ret can be in void function
  return std::make_unique<ReturnNode>( return_marker.position_, tryBuildExpression() );
}

std::optional<Type> Parser::tryBuildType() {
  Token first_supposed_type_token = current_token_;
  std::optional<Type> built_type = std::nullopt;
  switch ( current_token_.type_ ) {
    case TokenType::T_BOOL: built_type = Type{ BaseType::BOOL }; break;
    case TokenType::T_CHAR: built_type = Type{ BaseType::CHAR }; break;
    case TokenType::T_FLOAT: built_type = Type{ BaseType::FLOAT }; break;
    case TokenType::T_INT: built_type = Type{ BaseType::INT }; break;
    case TokenType::T_STR: built_type = Type{ ArrayType{ std::make_unique<Type>( BaseType::CHAR ) } }; break;
    // case TokenType::LBRACKET: return tryBuildArrayType();
    default: break;
  }
  if ( !built_type ) {
    assert( first_supposed_type_token == current_token_
            && "When 'Type' evalutes 'ε' 'current_token_' mustn't change but it did" );
    return std::nullopt;
  } else {
    nextToken();  // consume T_*
  }
  while ( current_token_.type_ == TokenType::LBRACKET ) {
    nextToken();
    consumeSpecificTokenOrThrow<MissingBracketException>( TokenType::RBRACKET, " ", BracketType::CLOSING );
    built_type = Type{ ArrayType{ std::make_unique<Type>( *std::move( built_type ) ) } };
  }
  return std::move( built_type );
}

// std::optional<Type> Parser::tryBuildArrayType() {
//   static constexpr std::string_view object_being_built_tag = "array type";
//   if ( current_token_.type_ != TokenType::LBRACKET ) return std::nullopt;
//   Token type_beg_marker = current_token_;
//   nextToken();  // consume opening bracket
//   auto underlying_type = tryBuildType();
//   if ( !underlying_type ) throw InvalidTypeException( type_beg_marker.position_, object_being_built_tag );
//   // no consuming type here, underlying_type already consumes the inside
//   consumeSpecificTokenOrThrow<MissingBracketException>( TokenType::RBRACKET, object_being_built_tag,
//                                                         BracketType::CLOSING );
//   return Type{ ArrayType{ std::make_unique<Type>( std::move( *underlying_type ) ) } };
// }

std::unique_ptr<IExpressionNode> Parser::tryBuildExpression() {
  auto left_node = tryBuildLogicalOrExpr();
  if ( !left_node ) return nullptr;
  if ( !parser_helper::isAssignment( current_token_.type_ ) ) return left_node;
  Token assignemt_token = current_token_;
  nextToken();  // consume "assign"
  auto right_node = tryBuildLogicalOrExpr();
  if ( !right_node ) {
    throw MissingRightOperandException( assignemt_token.position_,
                                        parser_helper::translateTokenTypeToAssignmentType( assignemt_token.type_ ) );
  }
  return std::make_unique<AssignmentExprNode>(
      assignemt_token.position_, std::move( left_node ), std::move( right_node ),
      parser_helper::translateTokenTypeToAssignmentType( assignemt_token.type_ ) );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildLogicalOrExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildLogicalAndExpr,
                             [&]( TokenType token_type ) -> bool { return token_type == TokenType::OP_OR; } );
};

std::unique_ptr<IExpressionNode> Parser::tryBuildLogicalAndExpr() {
  return tryBuildBinaryExpr( &Parser::tryBuildEqualityExpr,
                             [&]( TokenType token_type ) -> bool { return token_type == TokenType::OP_AND; } );
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
    nextToken();  // consume operator
    auto operand = tryBuildUnaryExpr();
    if ( !operand ) {
      throw MissingRightOperandException( operator_token.position_,
                                          parser_helper::translateTokenTypeToUnaryOperator( operator_token.type_ ) );
    }
    return std::make_unique<UnaryExprNode>( operator_token.position_, std::move( operand ),
                                            parser_helper::translateTokenTypeToUnaryOperator( operator_token.type_ ) );
  }
  return tryBuildCastExpr();
};

std::unique_ptr<IExpressionNode> Parser::tryBuildCastExpr() {
  auto left_node = tryBuildAccessExpr();
  if ( !left_node ) return nullptr;
  while ( current_token_.type_ == TokenType::KW_CAST_TO ) {
    Token operator_token = current_token_;
    nextToken();  // consume operator
    auto type = tryBuildType();
    if ( !type ) {
      throw InvalidTypeException( operator_token.position_, "cast_to expression" );
    }
    left_node = std::make_unique<CastExprNode>( operator_token.position_, std::move( left_node ), *std::move( type ) );
  }
  return left_node;
}

std::unique_ptr<IExpressionNode> Parser::tryBuildAccessExpr() {
  auto left_node = tryBuildPrimaryExpr();
  if ( !left_node ) return nullptr;
  while ( parser_helper::isAccesExprSufBeg( current_token_.type_ ) ) {
    Token operator_token = current_token_;
    std::unique_ptr<IExpressionNode> right_node = nullptr;

    switch ( current_token_.type_ ) {
      case TokenType::LBRACKET: {
        nextToken();  // consume operator
        right_node = tryBuildExpression();
        if ( !right_node ) throw MissingRightOperandException( operator_token.position_, "[]" );
        consumeSpecificTokenOrThrow<MissingBracketException>( TokenType::RBRACKET, "access expr",
                                                              BracketType::CLOSING );
        left_node = std::make_unique<BinaryExprNode>( operator_token.position_, std::move( left_node ),
                                                      std::move( right_node ), BinaryOperator::ACCESS );
        break;
      }
      case TokenType::OP_FILTER:
      case TokenType::OP_MAP: {
        nextToken();  // consume operator
        Token identifier_token_marker = current_token_;
        consumeSpecificTokenOrThrow<MissingIdentifierException>( TokenType::IDENTIFIER,
                                                                 "mapping to a function (access expr)" );
        left_node = std::make_unique<BinaryExprNode>(
            operator_token.position_, std::move( left_node ),
            std::make_unique<PrimaryIdentifierNode>( identifier_token_marker.position_,
                                                     std::get<std::string>( identifier_token_marker.value_ ) ),
            parser_helper::translateTokenTypeToBinaryOperator( operator_token.type_ ) );
        break;
      }
      default: return nullptr;
    }
  }
  return left_node;
}

std::unique_ptr<IExpressionNode> Parser::tryBuildPrimaryExpr() {
  if ( current_token_.type_ == TokenType::IDENTIFIER ) {
    return tryBuildFunCallOrReadExpr();
  } else if ( current_token_.type_ == TokenType::LPAREN ) {
    nextToken();  // lparen
    auto expr = tryBuildExpression();
    consumeSpecificTokenOrThrow<MissingParenthesisException>(
        TokenType::RPAREN, "primary expr: parenthesized expression", ParenthesisType::CLOSING );
    return expr;
  } else if ( parser_helper::isLiteral( current_token_.type_ ) ) {
    return tryBuildLiteralExpr();
  }
  return tryBuildArrayLiteralExpr();
}

std::unique_ptr<IExpressionNode> Parser::tryBuildFunCallOrReadExpr() {
  CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( TokenType::IDENTIFIER, identifier_token )

  if ( current_token_.type_ != TokenType::LPAREN ) {
    return std::make_unique<PrimaryIdentifierNode>( identifier_token.position_,
                                                    std::get<std::string>( std::move( identifier_token.value_ ) ) );
  }

  static constexpr std::string_view object_being_built_tag = "function call";

  consumeSpecificTokenOrThrow<MissingParenthesisException>( TokenType::LPAREN, object_being_built_tag,
                                                            ParenthesisType::OPENING );
  auto argument_list = tryBuildArgumentListExpr();
  consumeSpecificTokenOrThrow<MissingParenthesisException>( TokenType::RPAREN, object_being_built_tag,
                                                            ParenthesisType::CLOSING );
  return std::make_unique<FunctionCallNode>( identifier_token.position_,
                                             std::get<std::string>( std::move( identifier_token.value_ ) ),
                                             std::move( argument_list ) );
}

std::vector<std::unique_ptr<IExpressionNode>> Parser::tryBuildArgumentListExpr() {
  auto expr = tryBuildExpression();
  if ( !expr ) return {};
  std::vector<std::unique_ptr<IExpressionNode>> argument_list;
  argument_list.push_back( std::move( expr ) );

  while ( current_token_.type_ == TokenType::COMMA ) {
    nextToken();  // consume comma
    Token expr_beg_marker = current_token_;
    auto expr = tryBuildExpression();
    if ( !expr ) {
      throw MissingExpressionException( current_token_.position_,
                                        "argument list (Note: another argument required after comma)" );
    }
    argument_list.push_back( std::move( expr ) );
  }
  return std::move( argument_list );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildArrayLiteralExpr() {
  CONSUME_SPECIFIC_TOKEN_OR_RETURN_NULLPTR( TokenType::LBRACKET, opening_bracket_marker )
  auto array_positions = tryBuildArgumentListExpr();
  consumeSpecificTokenOrThrow<MissingBracketException>( TokenType::RBRACKET, "array literal", BracketType::CLOSING );
  return std::make_unique<ArrayLiteralNode>( opening_bracket_marker.position_, std::move( array_positions ) );
}

std::unique_ptr<IExpressionNode> Parser::tryBuildLiteralExpr() {
  std::unique_ptr<LiteralExprNode> literal_expr_node = nullptr;
  Token literal_beg_marker = current_token_;
  switch ( current_token_.type_ ) {
    case TokenType::BOOL_LITERAL:
      literal_expr_node = std::make_unique<LiteralExprNode>( current_token_.position_, Type{ BaseType::BOOL },
                                                             Value{ std::get<bool>( current_token_.value_ ) } );
      break;
    case TokenType::CHAR_LITERAL:
      literal_expr_node = std::make_unique<LiteralExprNode>( current_token_.position_, Type{ BaseType::CHAR },
                                                             Value{ std::get<char>( current_token_.value_ ) } );
      break;
    case TokenType::FLOAT_LITERAL:
      literal_expr_node = std::make_unique<LiteralExprNode>( current_token_.position_, Type{ BaseType::FLOAT },
                                                             Value{ std::get<float>( current_token_.value_ ) } );
      break;
    case TokenType::INT_LITERAL:
      literal_expr_node = std::make_unique<LiteralExprNode>( current_token_.position_, Type{ BaseType::INT },
                                                             Value{ std::get<int>( current_token_.value_ ) } );
      break;
    case TokenType::STRING_LITERAL: {
      std::string str_lit = std::get<std::string>( std::move( current_token_.value_ ) );
      literal_expr_node = std::make_unique<LiteralExprNode>(
          current_token_.position_, Type{ ArrayType{ std::make_unique<Type>( BaseType::CHAR ) } },
          Value{ std::vector<Value>( str_lit.begin(), str_lit.end() ) } );
      break;
    }
    default: break;
  }
  if ( !literal_expr_node ) {
    assert( literal_beg_marker == current_token_ && "" );
  } else {
    nextToken();  // consume literal token
  }
  return literal_expr_node;
}

std::optional<Token> Parser::consumeSpecificTokenOrReturnNull( const TokenType expected_token_type ) {
  if ( current_token_.type_ != expected_token_type ) {
    return std::nullopt;
  }
  Token matched_token = current_token_;
  nextToken();
  return matched_token;
}

Parser::Parser( ILexer& lexer ) : lexer_( lexer ), current_token_( getFirstToken() ) {
}

std::unique_ptr<ProgramNode> Parser::buildProgram() {
  std::vector<std::unique_ptr<INode>> statement_list = tryBuildStatementList();
  consumeSpecificTokenOrThrow<NotConsumedTokensException>( TokenType::END_OF_FILE, current_token_ );
  return std::make_unique<ProgramNode>( Position{ 1, 1 }, std::move( statement_list ) );
};
