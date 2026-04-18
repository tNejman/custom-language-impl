#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnterminatedCharLiteralException : public LexerException {
 private:
  const char c_found_;

 public:
  UnterminatedCharLiteralException( const Position pos, const char c_found )
      : LexerException( pos ), c_found_( c_found ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Unterminated char literal.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{
        std::format( "Expected a closing single quote but found '{}' instead.", c_found_ ) };
    return msg_addon;
  }
};