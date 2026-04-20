#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnterminatedStringLiteralException : public LexerException {
 public:
  UnterminatedStringLiteralException( const Position pos, const std::string string_beg )
      : LexerException( pos, std::format( "Unterminated string literal \"{}\". Expected closing quote before the end "
                                          "of the line. (Note: Rusthon++ strings cannot span multiple lines)\n"
                                          "{}\x1b[31m{}^\x1b[0m\n",
                                          string_beg, std::string( 29, ' ' ),  // "Unterminated string literal \""
                                          std::string( string_beg.empty() ? 0 : string_beg.length() - 1, '~' ) ) ) {
  }
};