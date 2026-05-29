#pragma once

#include "Exceptions/InterpreterExceptions/_InterpreterException.hpp"

class ConcreteInterpreterException : public InterpreterException {
 private:
 public:
  ConcreteInterpreterException( const Position pos, const std::string float_literal_str )
      : InterpreterException( pos, "" ) {
  }
};