#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnknownEscapedCharacterException : public LexerException {
 public:
  UnknownEscapedCharacterException( const Position pos, const char c )
      : LexerException(
            pos, std::format( "Unknown escaped char '\x1b[31m{}\x1b[0m'. Supported escapes are \\', \\\", \\\\, \\n.\n"
                              "                      \x1b[31m^\x1b[0m\n",
                              c ) ) {
  }
};