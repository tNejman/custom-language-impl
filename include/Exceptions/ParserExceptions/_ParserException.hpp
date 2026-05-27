#pragma once

#include <format>
#include <string>

#include "Drivers/Formatter.hpp"
#include "Lexer/Token.hpp"

class ParserException : public std::runtime_error {
 public:
  ParserException( const Position position, const std::string& derived_message )
      : std::runtime_error( std::format( "Parser Exception at {}\n{}", position, derived_message ) ) {
  }
  virtual ~ParserException() = default;
  ParserException( const ParserException& ) = delete;
  ParserException& operator=( const ParserException& ) = delete;
};