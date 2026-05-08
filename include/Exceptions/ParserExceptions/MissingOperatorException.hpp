#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class MissingOperatorException : public ParserException {
 private:
 public:
  MissingOperatorException( const Position pos, const std::string_view op, const std::string_view object_being_built )
      : ParserException( pos, std::format( "Missing Operator '{}' when building '{}'.", op, object_being_built ) ) {
  }
};