#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnterminatedStringLiteralException : public LexerException {
 public:
  UnterminatedStringLiteralException( const Position pos, const std::string string_beg )
      : LexerException( pos, std::format( "Unterminated string literal \"{}\". Expected closing quote before the end "
                                          "of the line. (Note: Rusthon++ strings cannot span "
                                          "multiple lines)",
                                          string_beg ) ) {
  }
};