#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class MissingExpressionException : public ParserException {
 private:
 public:
  MissingExpressionException( const Position pos, const std::string_view object_built_at_the_time )
      : ParserException( pos, std::format( "Missing Expression when building '{}'.", object_built_at_the_time ) ) {
  }
};