#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class ErrorHandler {
 private:
 public:
  bool mustTerminate( ParserException );
};