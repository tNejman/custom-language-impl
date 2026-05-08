#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class MissingIdentifierException : public ParserException {
 private:
 public:
  MissingIdentifierException( const Position pos, const std::string_view object_built_at_the_time )
      : ParserException( pos, std::format( "Missing Identifier when building '{}'.", object_built_at_the_time ) ) {
  }
};