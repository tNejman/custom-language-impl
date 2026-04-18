#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class InvalidCharLiteralException : public LexerException {
 private:
 public:
  InvalidCharLiteralException( const Position pos ) : LexerException( pos ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Invalid char literal.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{ std::format( "Expected a character between single quotes." ) };
    return msg_addon;
  }
};