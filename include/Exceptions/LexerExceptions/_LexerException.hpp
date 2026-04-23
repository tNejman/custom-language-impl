#pragma once

#include <format>
#include <string>

#include "Drivers/Formatter.hpp"
#include "Lexer/Token.hpp"

class LexerException : public std::runtime_error {
 public:
  LexerException( const Position position, const std::string& derived_message )
      : std::runtime_error( std::format( "Lexer Exception at {}\n{}", position, derived_message ) ) {
  }
  virtual ~LexerException() = default;
  LexerException( const LexerException& ) = delete;
  LexerException& operator=( const LexerException& ) = delete;
};