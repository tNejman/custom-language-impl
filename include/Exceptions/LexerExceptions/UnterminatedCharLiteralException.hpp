#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnterminatedCharLiteralException : public LexerException {
 public:
  UnterminatedCharLiteralException( const Position pos, const char c_found )
      : LexerException(
            pos, std::format( "Unterminated char literal. Expected a closing single quote but found '{}' instead.",
                              c_found ) ) {
  }
};