#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnterminatedStringLiteralException : public LexerException {
 private:
  const std::string string_beg_;

 public:
  UnterminatedStringLiteralException( const Position pos, const std::string string_beg )
      : LexerException( pos ), string_beg_( string_beg ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Unterminated string literal.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{
        std::format( "\"{}\" Expected closing quote before the end of the line. (Note: Rusthon++ strings cannot span "
                     "multiple lines)",
                     string_beg_ ) };
    return msg_addon;
  }
};