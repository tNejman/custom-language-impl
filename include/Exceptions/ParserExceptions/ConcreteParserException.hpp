#pragma once

#include <float.h>

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class ConcreteParserException : public ParserException {
 private:
 public:
  ConcreteParserException( const Position pos, const std::string float_literal_str ) : ParserException( pos, "" ) {
  }
};