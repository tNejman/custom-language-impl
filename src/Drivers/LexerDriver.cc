#include "Drivers/LexerDriver.h"

#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <sstream>

#include "ArgParser.hpp"
#include "Exceptions/LexerExceptions/_LexerException.hpp"

BatchLexerDriver::BatchLexerDriver( std::unique_ptr<std::istream> stream )
    : stream_( std::move( stream ) ), lexer_( *stream_ ) {
}

void BatchLexerDriver::run() {
  while ( true ) {
    try {
      Token t = lexer_.getNextToken();
      std::cout << std::format( "{}", t );
      if ( t.type_ == TokenType::END_OF_FILE ) break;
    } catch ( const LexerException &e ) {
      std::cout << e.what() << "Terminating after error." << std::endl;
      std::exit( 1 );
    } catch ( const std::exception &e ) {
      std::cout << "DEBUG: unexpected system exception: " << e.what();
      std::exit( 1 );
    }
  }
  std::cout << "Whole file read." << std::endl;
}

InteractiveLexerDriver::InteractiveLexerDriver() : lexer_( std::cin ) {
}

void InteractiveLexerDriver::run() {
  while ( true ) {
    std::cout << "Rusthon++ > " << std::flush;
    try {
      while ( true ) {
        Token t = lexer_.getNextToken();
        std::cout << std::format( "{}", t );
        if ( t.type_ == TokenType::END_OF_FILE || t.type_ == TokenType::NEWLINE ) {
          break;
        }
      }
    } catch ( const LexerException &e ) {
      std::cout << e.what();
      if ( std::cin.fail() ) {
        std::cin.clear();
      }
      std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
      std::cin.putback( '\n' );
      std::cout << std::format( "{}", lexer_.getNextToken() );
    } catch ( const std::exception &e ) {
      std::cout << "DEBUG: unexpected system exception: " << e.what();
      std::exit( 1 );
    }
  }
}

namespace driver_factory {
std::unique_ptr<ILexerDriver> build( const ArgParseLexerResult &config ) {
  std::unique_ptr<std::istream> stream;

  switch ( config.mode ) {
    case LexerInputSource::FILE: {
      auto file_stream = std::make_unique<std::ifstream>( config.filepath );
      if ( !file_stream->is_open() ) {
        std::cerr << "Unable to open file: " << config.filepath << std::endl;
        std::exit( 1 );
      }
      stream = std::move( file_stream );
      return std::make_unique<BatchLexerDriver>( std::move( stream ) );
    }

    case LexerInputSource::ARG: {
      stream = std::make_unique<std::istringstream>( config.str_argument );
      return std::make_unique<BatchLexerDriver>( std::move( stream ) );
    }

    case LexerInputSource::CIN: {
      return std::make_unique<InteractiveLexerDriver>();
    }
  }

  return nullptr;
}
};  // namespace driver_factory