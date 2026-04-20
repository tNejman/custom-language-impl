#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnknownSymbolException : public LexerException {
 public:
  UnknownSymbolException( const Position pos, const char c )
      : LexerException(
            pos, std::format( "Unknown character '\x1b[31m{}\x1b[0m'. This character has no meaning in Rusthon++.\n"
                              "                   \x1b[31m^\x1b[0m\n",
                              c ) ) {
  }
};