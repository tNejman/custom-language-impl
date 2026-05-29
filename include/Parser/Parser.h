#pragma once

#include <concepts>
#include <type_traits>

#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"  // IWYU pragma: keep
#include "Lexer/ILexer.h"
#include "Lexer/Token.hpp"
#include "Parser/IParser.hpp"
#include "Parser/Node.h"
#include "Parser/ParserHelper.h"

using StmtFunVecPair = std::pair<std::vector<std::unique_ptr<INode>>, std::vector<std::unique_ptr<FunctionDefNode>>>;

class Parser : public IParser {
 private:
  ILexer& lexer_;
  Token current_token_;

  Token getFirstToken();

  Token nextToken();
  void skipComments();
  void skipNewlines();

  StmtFunVecPair tryBuildStmtOrFunDefList();
  NextParsedNode tryBuildNextNode();
  std::unique_ptr<INode> tryBuildStatement();

  std::unique_ptr<FunctionDefNode> tryBuildFunctionDef();
  std::optional<Type> tryBuildReturnType();
  std::vector<ParameterDecl> tryBuildParamList();
  std::optional<ParameterDecl> tryBuildParameter();
  Block tryBuildBlock();
  std::vector<std::unique_ptr<INode>> tryBuildStatementList();

  std::unique_ptr<INode> tryBuildVariableDecl( const Mutability mutability );

  std::unique_ptr<INode> tryBuildIfStmt();
  std::pair<std::unique_ptr<IExpressionNode>, Block> tryBuildElseIfBranch();
  Block tryBuildElseBlock();
  std::pair<std::unique_ptr<IExpressionNode>, Block> tryBuildParenthesizedExpressionAndBlock();

  std::unique_ptr<INode> tryBuildWhileStmt();

  std::unique_ptr<INode> tryBuildControlFlowStmt();
  std::unique_ptr<INode> tryBuildReturnStmt();
  std::optional<Type> tryBuildType();

  std::unique_ptr<IExpressionNode> tryBuildExpression();
  std::unique_ptr<IExpressionNode> tryBuildAssignmentExpr();
  std::unique_ptr<IExpressionNode> tryBuildLogicalOrExpr();
  std::unique_ptr<IExpressionNode> tryBuildLogicalAndExpr();
  std::unique_ptr<IExpressionNode> tryBuildEqualityExpr();
  std::unique_ptr<IExpressionNode> tryBuildRelationalExpr();
  std::unique_ptr<IExpressionNode> tryBuildAdditiveExpr();
  std::unique_ptr<IExpressionNode> tryBuildMultiplicativeExpr();
  std::unique_ptr<IExpressionNode> tryBuildUnaryExpr();
  std::unique_ptr<IExpressionNode> tryBuildCastExpr();
  std::unique_ptr<IExpressionNode> tryBuildAccessExpr();
  std::unique_ptr<IExpressionNode> tryBuildPrimaryExpr();
  std::unique_ptr<IExpressionNode> tryBuildFunCallOrReadExpr();
  std::vector<std::unique_ptr<IExpressionNode>> tryBuildArgumentListExpr();
  std::unique_ptr<IExpressionNode> tryBuildArrayLiteralExpr();
  std::unique_ptr<IExpressionNode> tryBuildLiteralExpr();

  std::unique_ptr<IExpressionNode> tryBuildParenExpr();
  std::unique_ptr<IExpressionNode> tryBuildAccessArrayExpr( std::unique_ptr<IExpressionNode>& left_node );
  std::unique_ptr<IExpressionNode> tryBuildFilterOrMapAccessExpr( std::unique_ptr<IExpressionNode>& left_node );

  using NextLevelMethod = std::unique_ptr<IExpressionNode> ( Parser::* )();

  template <typename MatchOperatorFunction>
  requires std::invocable<MatchOperatorFunction, TokenType>
           && std::same_as<std::invoke_result_t<MatchOperatorFunction, TokenType>, bool>
  std::unique_ptr<IExpressionNode> tryBuildBinaryExpr( NextLevelMethod next_lvl_meth,
                                                       MatchOperatorFunction match_op_fun ) {
    auto left_node = ( this->*next_lvl_meth )();
    if ( !left_node ) return nullptr;
    while ( match_op_fun( current_token_.type_ ) ) {
      Token operator_token = current_token_;
      nextToken();  // consume operator
      auto right_node = ( this->*next_lvl_meth )();
      if ( !right_node ) {
        throw MissingRightOperandException( operator_token.position_,
                                            parser_helper::translateTokenTypeToBinaryOperator( operator_token.type_ ) );
      }
      left_node =
          std::make_unique<BinaryExprNode>( operator_token.position_, std::move( left_node ), std::move( right_node ),
                                            parser_helper::translateTokenTypeToBinaryOperator( operator_token.type_ ) );
    }
    return left_node;
  }

  template <typename ParCustException, typename... Args>
  requires std::is_constructible_v<ParCustException, Position, Args...>
  void consumeSpecificTokenOrThrow( const TokenType to_be_matched, Args&&... exc_args ) {
    if ( current_token_.type_ != to_be_matched ) {
      throw ParCustException( current_token_.position_, std::forward<Args>( exc_args )... );
    }
    nextToken();
  }

  template <typename ParCustException, typename MatchTokenTypeFunction, typename... Args>
  requires std::is_constructible_v<ParCustException, Position, Args...>
           && std::invocable<MatchTokenTypeFunction, TokenType>
           && std::same_as<std::invoke_result_t<MatchTokenTypeFunction, TokenType>, bool>
  void consumeTokenFromCategoryOrThrow( MatchTokenTypeFunction match_token_type_fun, Args&&... exc_args ) {
    if ( !match_token_type_fun( current_token_.type_ ) ) {
      throw ParCustException( current_token_.position_, std::forward<Args>( exc_args )... );
    }
    nextToken();
  }

  std::optional<Token> consumeSpecificTokenOrReturnNull( const TokenType expected_token_type );

 public:
  Parser( ILexer& lexer );
  std::unique_ptr<ProgramNode> buildProgram() override;
};