#pragma once

#include <float.h>

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class FloatLiteralOutOfBoundsException : public LexerException {
 private:
 public:
  FloatLiteralOutOfBoundsException( const Position pos, const std::string float_literal_str )
      : LexerException(
            pos, std::format( "Float literal '{}' out of bounds. (Note: Largest float magnitude is {} "
                              "(regardless of sign). Smallest float magnitude is {} (closest to zero).)\n"
                              "{}\x1b[31m{}^\x1b[0m\n",
                              float_literal_str, FLT_MAX, FLT_MIN, std::string( 15, ' ' ),
                              std::string( float_literal_str.empty() ? 0 : float_literal_str.length() - 1, '~' ) ) ) {
  }
};