#pragma once

#include "Lexer/Token.hpp"

class ILexer {
 public:
  ILexer() = default;
  virtual ~ILexer() = default;
  ILexer( const ILexer& ) = delete;
  ILexer& operator=( const ILexer& ) = delete;

  virtual Token getNextToken() = 0;
};
