#pragma once

#include <float.h>

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class FloatLiteralOutOfBoundsException : public LexerException {
 private:
 public:
  FloatLiteralOutOfBoundsException( const Position pos, const std::string float_literal_str )
      : LexerException( pos, std::format( "Float literal '{}' out of bounds. (Note: Largetst float magnitude is {} "
                                          "(regardless of sign). Smallest flaot "
                                          "magnitude is "
                                          "{} (closest to zero).)",
                                          float_literal_str, FLT_MAX, FLT_MIN ) ) {
  }
};