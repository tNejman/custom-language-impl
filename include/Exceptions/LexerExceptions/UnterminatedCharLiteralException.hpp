#pragma once

#include <exception>
#include <string>

class UnterminatedCharLiteralException : public std::exception {
 private:
  std::string message_;

 public:
  UnterminatedCharLiteralException( const std::string& msg ) : message_( msg ) {
  }

  const char* what() const noexcept override {
    return message_.c_str();
  }
};