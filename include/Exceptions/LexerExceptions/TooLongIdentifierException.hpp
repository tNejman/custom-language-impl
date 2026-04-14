#pragma once

#include <exception>
#include <string>

class TooLongIdentifierException : public std::exception {
 private:
  std::string message_;

 public:
  TooLongIdentifierException( const std::string& msg ) : message_( msg ) {
  }

  const char* what() const noexcept override {
    return message_.c_str();
  }
};