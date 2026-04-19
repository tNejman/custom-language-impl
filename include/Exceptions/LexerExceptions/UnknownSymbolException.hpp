#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnknownSymbolException : public LexerException {
 public:
  UnknownSymbolException( const Position pos, const char c )
      : LexerException( pos, std::format( "Unknown character '{}' This character has no meaning in Rusthon++.", c ) ) {
  }
};