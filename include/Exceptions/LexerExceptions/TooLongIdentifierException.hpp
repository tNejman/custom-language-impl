#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class TooLongIdentifierException : public LexerException {
 public:
  TooLongIdentifierException( const Position pos, const std::string identifier_beg )
      : LexerException( pos,
                        std::format( "Too long identifier '{}'. Maximum {} characters allowed.\n"
                                     "{}\x1b[31m{}^\x1b[0m\n",
                                     identifier_beg, MAX_IDENTIFIER_LENGTH, std::string( 21, ' ' ),
                                     std::string( identifier_beg.empty() ? 0 : identifier_beg.length() - 1, '~' ) ) ) {
  }
};