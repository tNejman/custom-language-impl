#include <fstream>
#include <memory>
#include <sstream>

#include "ArgParser.hpp"
#include "Drivers/LexerDriver.h"

int main( int argc, const char** argv ) {
  ArgParseLexerResult config = parseLexerArguments( argc, argv );

  std::unique_ptr<ILexerDriver> driver;
  std::unique_ptr<std::istream> stream;
  std::unique_ptr<Lexer> lexer;
  switch ( config.mode ) {
    case LexerInputSource::FILE: {
      auto file_stream = std::make_unique<std::ifstream>( config.filepath );
      if ( !file_stream->is_open() ) {
        std::cerr << "Unable to open file: " << config.filepath << std::endl;
        return 1;
      }
      stream = std::move( file_stream );
      lexer = std::make_unique<Lexer>( *stream );
      driver = std::make_unique<BatchLexerDriver>( *lexer );
      break;
    }
    case LexerInputSource::ARG: {
      stream = std::make_unique<std::istringstream>( config.str_argument );
      lexer = std::make_unique<Lexer>( *stream );
      driver = std::make_unique<BatchLexerDriver>( *lexer );
      break;
    }
    case LexerInputSource::CIN: {
      lexer = std::make_unique<Lexer>( std::cin );
      driver = std::make_unique<InteractiveLexerDriver>( *lexer );
      break;
    }
  }
  if ( driver ) driver->run();

  return 0;
}