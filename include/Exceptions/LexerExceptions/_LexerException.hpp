#pragma once

#include <exception>
#include <format>
#include <string>

#include "Lexer/Token.hpp"

class LexerException : public std::exception {
 private:
  const Position position_;

 protected:
  std::string full_message_;

 public:
  LexerException( const Position pos, std::string derived_message )
      : position_( pos ),
        full_message_( std::format( "Lexer Exception at {}\n{}", position_.toString(), derived_message ) ) {
  }
  virtual ~LexerException() = default;
  LexerException( const LexerException& ) = delete;
  LexerException* operator=( const LexerException& ) = delete;

  virtual const char* what() const noexcept override {
    return full_message_.c_str();
  }
};