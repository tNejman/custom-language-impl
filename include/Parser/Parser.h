#pragma once

#include "Parser/IParser.hpp"
#include "Parser/Node.hpp"
#include "Lexer/Token.hpp"
#include "Lexer/Lexer.h"

class Parser : public IParser {
 private:
  Token current_token_;
  Lexer& lexer_;

  void nextToken();

  std::unique_ptr<INode> buildStatement();
  std::unique_ptr<INode> buildFunctionDef();
  std::unique_ptr<INode> buildVariableDecl(const bool is_mutable);
  std::unique_ptr<INode> buildIfStmt();

 public:
  std::unique_ptr<INode> parse() override;
};