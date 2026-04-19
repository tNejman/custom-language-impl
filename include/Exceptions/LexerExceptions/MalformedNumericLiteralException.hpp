#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class MalformedNumericLiteralException : public LexerException {
 public:
  MalformedNumericLiteralException( const Position pos, const char c, const std::string numeric_literal_str )
      : LexerException(
            pos,
            std::format(
                "Malformed numeric literal. Invalid character '{}' in numeric literal '{}'. Expected digits or '_'.", c,
                numeric_literal_str ) ) {
  }
};