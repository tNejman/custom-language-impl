#pragma once

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class TooLongIdentifierException : public LexerException {
 private:
  const std::string identifier_beg_;

 public:
  TooLongIdentifierException( const Position pos, const std::string identifier_beg )
      : LexerException( pos ), identifier_beg_( identifier_beg ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Too long identifier.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{
        std::format( "\"{}\" Maximum {} characters allowed.", identifier_beg_, MAX_IDENTIFIER_LENGTH ) };
    return msg_addon;
  }
};