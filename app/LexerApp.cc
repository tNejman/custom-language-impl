#include "ArgParser.hpp"
#include "Lexer/LexerDriver.h"

int main( int argc, const char** argv ) {
  ArgParseLexerResult config = parseLexerArguments( argc, argv );
  LexerDriver driver{ config.mode, config.source };
  driver.run();
}