#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class TooLongStringLiteralException : public LexerException {
 public:
  TooLongStringLiteralException( const Position pos, const std::string string_beg )
      : LexerException( pos, std::format( "Too long string literal '{}'. Maximum {} characters allowed.", string_beg,
                                          MAX_STRING_LITERAL_LENGTH ) ) {
  }
};