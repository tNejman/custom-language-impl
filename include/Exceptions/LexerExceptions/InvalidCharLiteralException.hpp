#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class InvalidCharLiteralException : public LexerException {
 private:
 public:
  InvalidCharLiteralException( const Position pos )
      : LexerException( pos,
                        "Invalid char literal ''. Expected a character between single quotes.\n"
                        "                      \x1b[31m^\x1b[0m\n" ) {
  }
};