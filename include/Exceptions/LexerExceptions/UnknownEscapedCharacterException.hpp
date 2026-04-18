#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnknownEscapedCharacterException : public LexerException {
 private:
  const char c_;

 public:
  UnknownEscapedCharacterException( const Position pos, const char c ) : LexerException( pos ), c_( c ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Unknown escaped char.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{ std::format( "'{}' Supported escapes are \', \", \\, \n.", c_ ) };
    return msg_addon;
  }
};