#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class TooLongStringLiteralException : public LexerException {
 public:
  TooLongStringLiteralException( const Position pos, const std::string string_beg )
      : LexerException( pos, std::format( "Too long string literal '{}'. Maximum {} characters allowed.\n"
                                          "{}\x1b[31m{}^\x1b[0m\n",
                                          string_beg, MAX_STRING_LITERAL_LENGTH, std::string( 25, ' ' ),
                                          std::string( string_beg.empty() ? 0 : string_beg.length() - 1, '~' ) ) ) {
  }
};