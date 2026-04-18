#pragma once

#include <float.h>

#include "Exceptions/LexerExceptions/_LexerException.hpp"

class FloatLiteralOutOfBoundsException : public LexerException {
 private:
 public:
  FloatLiteralOutOfBoundsException( const Position pos ) : LexerException( pos ) {
  }

  virtual std::string_view getExceptionName() const override {
    return "Float literal out of bounds.";
  }
  virtual std::string_view getMessageAddon() const override {
    static const std::string msg_addon{ std::format(
        "Largetst float magnitude is {} (regardless of sign). Smallest flaot magnitude is {} (closest to zero).",
        FLT_MAX, FLT_MIN ) };
    return msg_addon;
  }
};