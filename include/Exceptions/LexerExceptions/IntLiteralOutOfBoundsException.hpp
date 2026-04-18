#pragma once

#include <climits>

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class IntLiteralOutOfBoundsException : public LexerException {
 private:
 public:
  IntLiteralOutOfBoundsException( const Position pos ) : LexerException( pos ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Int literal out of bounds.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{ std::format( "Max int value is {}, min int value is {}", INT_MAX, INT_MIN ) };
    return msg_addon;
  }
};