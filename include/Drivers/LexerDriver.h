#pragma once

#include "Lexer/Lexer.h"

class ILexerDriver {
 private:
  Lexer& lexer_;

 protected:
  Lexer& getLexer();

 public:
  ILexerDriver( Lexer& );
  virtual ~ILexerDriver() = default;
  virtual void run() = 0;
};

class BatchLexerDriver : public ILexerDriver {
 public:
  explicit BatchLexerDriver( Lexer& );
  void run() override;
};

class InteractiveLexerDriver : public ILexerDriver {
 public:
  explicit InteractiveLexerDriver( Lexer& );
  void run() override;
};