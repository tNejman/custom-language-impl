#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnknownEscapedCharacterException : public LexerException {
 public:
  UnknownEscapedCharacterException( const Position pos, const char c )
      : LexerException( pos, std::format( "Unknown escaped char '{}'. Supported escapes are \', \", \\, \n.", c ) ) {
  }
};