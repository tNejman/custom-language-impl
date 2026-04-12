#pragma once

#include <istream>

#include "Lexer/ILexer.h"

class Lexer : public ILexer {
 private:
  std::istream& input_stream_;
  Position current_pos_{ 0, 0 };
  char current_char_{ '\0' };

  void nextChar();

 public:
  explicit Lexer( std::istream& input );

  Token getNextToken() override;
};