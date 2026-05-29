#pragma once

#include <format>
#include <string>

#include "Drivers/Formatter.hpp"
#include "Lexer/Token.hpp"

class InterpreterException : public std::runtime_error {
 public:
  InterpreterException( const Position position, const std::string& derived_message )
      : std::runtime_error( std::format( "Interpreter Exception at {}\n{}", position, derived_message ) ) {
  }
  virtual ~InterpreterException() = default;
  InterpreterException( const InterpreterException& ) = delete;
  InterpreterException& operator=( const InterpreterException& ) = delete;
};