#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class InvalidCommentStyleException : public LexerException {
 private:
 public:
  InvalidCommentStyleException( const Position pos )
      : LexerException( pos,
                        "Invalid comment style: '/* ... */'. (Note: Rusthon++ supports only single line comments "
                        "starting with symbol '#'.)" ) {
  }
};