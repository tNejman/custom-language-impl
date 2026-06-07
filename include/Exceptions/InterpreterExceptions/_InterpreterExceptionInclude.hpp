#pragma once

#include "Exceptions/InterpreterExceptions/_InterpreterException.hpp"

class FunctionSignatureMismatchException : public InterpreterException {
 public:
  FunctionSignatureMismatchException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class VoidValueException : public InterpreterException {
 public:
  VoidValueException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class NotAllowedTypeException : public InterpreterException {
 public:
  NotAllowedTypeException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class InvalidCastException : public InterpreterException {
 public:
  InvalidCastException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class InvalidOperationException : public InterpreterException {
 public:
  InvalidOperationException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class IndexOutOfBoundsException : public InterpreterException {
 public:
  IndexOutOfBoundsException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class InvalidAccessException : public InterpreterException {
 public:
  InvalidAccessException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class InvalidOverloadException : public InterpreterException {
 public:
  InvalidOverloadException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};
class InvalidShadowException : public InterpreterException {
 public:
  InvalidShadowException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class UnknownIdentifierException : public InterpreterException {
 public:
  UnknownIdentifierException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

/*

*/

class EmptyAccumulatorException : public InterpreterException {
 public:
  EmptyAccumulatorException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};
class BuiltinFunctionInitException : public InterpreterException {
 public:
  BuiltinFunctionInitException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class InvalidStatementException : public InterpreterException {
 public:
  InvalidStatementException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};

class StackOverflowException : public InterpreterException {
 public:
  StackOverflowException( Position pos, const std::string& msg ) : InterpreterException( pos, msg ) {
  }
};