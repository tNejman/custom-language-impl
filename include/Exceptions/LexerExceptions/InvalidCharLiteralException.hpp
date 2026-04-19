#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class InvalidCharLiteralException : public LexerException {
 private:
 public:
  InvalidCharLiteralException( const Position pos )
      : LexerException( pos, "Invalid char literal ''. Expected a character between single quotes." ) {
  }
};