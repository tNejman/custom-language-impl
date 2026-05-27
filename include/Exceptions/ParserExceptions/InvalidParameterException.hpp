#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class InvalidParameterException : public ParserException {
 private:
 public:
  InvalidParameterException( const Position pos )
      : ParserException( pos,
                         "Couldn't construct a valid Parameter Declaration at given position. (Note: a comma in "
                         "function declaration must be followed by another parameter)" ) {
  }
};