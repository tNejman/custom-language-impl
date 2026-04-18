#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"
#include "Lexer/Token.hpp"

class TooLongStringLiteralException : public LexerException {
 private:
  const std::string string_beg_;

 public:
  TooLongStringLiteralException( const Position pos, const std::string string_beg )
      : LexerException( pos ), string_beg_( string_beg ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Too long string literal.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{
        std::format( "\"{}\" Maximum {} characters allowed.", string_beg_, MAX_STRING_LITERAL_LENGTH ) };
    return msg_addon;
  }
};