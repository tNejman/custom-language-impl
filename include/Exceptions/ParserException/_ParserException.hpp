#pragma once

#include <exception>

#include "Lexer/Token.hpp"

class ParserException : public std::exception {
 private:
  const Position position_;

 protected:
  std::string full_message_;

 public:
  ParserException( const Position pos, std::string derived_message )
      : position_( pos ),
        full_message_( std::format( "Parser Exception at {}\n{}", position_.toString(), derived_message ) ) {
  }
  virtual ~ParserException() = default;
  ParserException( const ParserException& ) = delete;
  ParserException* operator=( const ParserException& ) = delete;

  virtual const char* what() const noexcept override {
    return full_message_.c_str();
  }
};