#pragma once

#include <memory>

#include "Lexer/Lexer.h"

enum class LexerDriverMode { FILE, CIN, ARG };

class LexerDriver {
 private:
  std::unique_ptr<Lexer> lexer_;
  std::unique_ptr<std::istream> source_stream_;
  const LexerDriverMode mode_;

  void runOnFileInput();
  void runOnCinInput();
  void runOnArgInput();

 public:
  LexerDriver( LexerDriverMode mode, std::string source );

  void run();
};