#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class UnknownSymbolException : public LexerException {
 private:
  const char c_;

 public:
  UnknownSymbolException( const Position pos, const char c ) : LexerException( pos ), c_( c ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Unknown character.";
  }
  virtual std::string getMessageAddon() const override {
    const std::string msg_addon{ std::format( "'{}' This character has no meaning in Rusthon++.", c_ ) };
    return msg_addon;
  }
};