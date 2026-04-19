#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class TooLongIdentifierException : public LexerException {
 public:
  TooLongIdentifierException( const Position pos, const std::string identifier_beg )
      : LexerException( pos, std::format( "Too long identifier '{}'. Maximum {} characters allowed.", identifier_beg,
                                          MAX_IDENTIFIER_LENGTH ) ) {
  }
};