#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class MalformedNumericLiteralException : public LexerException {
 public:
  MalformedNumericLiteralException( const Position pos, const char c, const std::string numeric_literal_str )
      : LexerException( pos, std::format( "Malformed numeric literal. Invalid character in numeric literal "
                                          "'{}\x1b[31m{}\x1b[0m'. Expected digits or '_'.\n"
                                          "{}{}\x1b[31m^\x1b[0m\n",
                                          numeric_literal_str, c, std::string( 65, ' ' ),
                                          std::string( numeric_literal_str.length(), '~' ) ) ) {
  }
};