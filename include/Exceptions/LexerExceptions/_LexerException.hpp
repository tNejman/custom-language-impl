#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Lexer/Token.hpp"

class LexerException : public std::exception {
 private:
  const Position position_;
  const std::string full_message_;

  std::string generateMessage() const {
    const static std::string lexer_exception_message_base = std::format( "Lexer Exception: " );
  }

  virtual std::string_view getExceptionName() const = 0;
  virtual std::string_view getMessageAddon() const = 0;

 public:
  LexerException( const Position pos )
      : position_( pos ),
        full_message_( std::format( "Lexer Exception at {}\n: {}", position_.toString(), getExceptionName(),
                                    getMessageAddon() ) ) {
  }
  virtual ~LexerException() = default;
  LexerException( const LexerException& ) = delete;
  LexerException* operator=( const LexerException& ) = delete;

  virtual const char* what() const noexcept {
    return full_message_.c_str();
  }
};