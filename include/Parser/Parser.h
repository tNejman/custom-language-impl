#pragma once

#include "Lexer/Lexer.h"
#include "Lexer/Token.hpp"
#include "Parser/ErrorHandler.h"
#include "Parser/IParser.hpp"
#include "Parser/Node.hpp"

class Parser : public IParser {
 private:
  Token current_token_;
  Lexer lexer_;
  ErrorHandler error_handler_;

  void nextToken();
  void skipComments();

  std::unique_ptr<INode> tryBuildStatement();

  std::unique_ptr<INode> tryBuildFunctionDef();
  std::unique_ptr<Type> tryBuildReturnType();
  std::vector<Parameter> tryBuildParamList();
  std::unique_ptr<INode> tryBuildBlock();

  std::unique_ptr<INode> tryBuildVariableDecl( const bool is_mutable );
  std::unique_ptr<INode> tryBuildIfStmt();
  std::unique_ptr<INode> tryBuildWhileStmt();
  std::unique_ptr<INode> tryBuildControlFlowStmt();
  std::unique_ptr<INode> tryBuildReturnStmt();
  std::unique_ptr<INode> tryBuildExpressionStmt();

  std::unique_ptr<Type> tryBuildType();

 public:
  Program buildProgram() override;
};