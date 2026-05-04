#pragma once

#include <climits>

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class IntLiteralOutOfBoundsException : public LexerException {
 private:
 public:
  IntLiteralOutOfBoundsException( const Position pos, const std::string int_literal_str )
      : LexerException(
            pos, std::format( "Int literal '{}' out of bounds. (Note: Max int value is {}, min int value is {})\n"
                              "{}\x1b[31m{}^\x1b[0m\n",
                              int_literal_str, INT_MAX, INT_MIN, std::string( 13, ' ' ),
                              std::string( int_literal_str.empty() ? 0 : int_literal_str.length() - 1, '~' ) ) ) {
  }
};