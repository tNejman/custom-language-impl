#pragma once

#include "ArgParser.hpp"
#include "Lexer/Lexer.h"

class ILexerDriver {
 public:
  virtual ~ILexerDriver() = default;
  virtual void run() = 0;
};

class BatchLexerDriver : public ILexerDriver {
 private:
  std::unique_ptr<std::istream> stream_;
  Lexer lexer_;

 public:
  BatchLexerDriver( std::unique_ptr<std::istream> stream );
  void run() override;
};

class InteractiveLexerDriver : public ILexerDriver {
 private:
  Lexer lexer_;

 public:
  InteractiveLexerDriver();
  void run() override;
};

namespace driver_factory {
std::unique_ptr<ILexerDriver> build( const ArgParseLexerResult& config );
};