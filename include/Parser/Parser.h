#pragma once

#include <stdexcept>

#include "Lexer/Lexer.h"
#include "Lexer/Token.hpp"
#include "Parser/ErrorHandler.h"
#include "Parser/IParser.hpp"
#include "Parser/Node.h"
#include "Parser/ParserHelper.h"

class Parser : public IParser {
 private:
  Token current_token_;
  std::optional<Token> token_buffer_ = std::nullopt;
  Lexer lexer_;
  ErrorHandler error_handler_;

  void nextToken();
  Token peek();
  void skipComments();

  std::vector<std::unique_ptr<INode>> tryBuildStatementList();
  std::unique_ptr<INode> tryBuildStatement();

  std::unique_ptr<INode> tryBuildFunctionDef();
  Type tryBuildReturnType();
  std::vector<std::unique_ptr<ParameterDecl>> tryBuildParamList();
  std::unique_ptr<ParameterDecl> tryBuildParameter();
  Block tryBuildBlock();

  std::unique_ptr<INode> tryBuildVariableDecl( const Mutability mutability );

  std::unique_ptr<INode> tryBuildIfStmt();
  std::unique_ptr<INode> tryBuildElseIfBranch();
  std::pair<std::unique_ptr<IExpressionNode>, Block> tryBuildParenthesizedExpressionAndBlock();

  std::unique_ptr<INode> tryBuildWhileStmt();

  std::unique_ptr<INode> tryBuildControlFlowStmt();
  std::unique_ptr<INode> tryBuildReturnStmt();

  std::unique_ptr<IExpressionNode> tryBuildExpression();
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
  std::vector<std::unique_ptr<IExpressionNode>> tryBuildArgumentListExpr();
  std::unique_ptr<IExpressionNode> tryBuildArrayLiteralExpr();
  std::unique_ptr<IExpressionNode> tryBuildLiteralExpr();

  template <typename NextLevelFunction, typename MatchOperatorFunction>
  std::unique_ptr<IExpressionNode> tryBuildBinaryExpr( NextLevelFunction next_lvl_fun,
                                                       MatchOperatorFunction match_op_fun ) {
    auto left_node = ( this->*next_lvl_fun )();
    if ( !left_node ) return nullptr;
    while ( match_op_fun( peek().type_ ) ) {
      nextToken();
      Token operator_token = current_token_;

      auto right_node = ( this->*next_lvl_fun )();
      if ( !right_node ) throw std::runtime_error( "" );

      left_node =
          std::make_unique<BinaryExprNode>( std::move( left_node ), std::move( right_node ),
                                            parser_helper::translateTokenTypeToBinaryOperator( operator_token.type_ ) );
    }
  }

  std::optional<Type> tryBuildType();

 public:
  ProgramNode buildProgram() override;
};