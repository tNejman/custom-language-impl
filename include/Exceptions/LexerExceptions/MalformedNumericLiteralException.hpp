#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class MalformedNumericLiteralException : public LexerException {
 private:
  const char c_;

 public:
  MalformedNumericLiteralException( const Position pos, const char c ) : LexerException( pos ), c_( c ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Malformed numeric literal.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{
        std::format( "Invalid character '{}' in numeric literal; digits or '_' expected.", c_ ) };
    return msg_addon;
  }
};