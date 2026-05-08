#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class MissingNewlineException : public ParserException {
 private:
 public:
  MissingNewlineException( const Position pos, const std::string_view object_after_which_is_missing )
      : ParserException( pos, std::format( "Missing newline after '{}'.", object_after_which_is_missing ) ) {
  }
};