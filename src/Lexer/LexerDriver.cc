
#include "Lexer/LexerDriver.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Exceptions/LexerExceptions/_LexerException.hpp"
#include "Token.hpp"

LexerDriver::LexerDriver( LexerDriverMode mode, std::string source ) : mode_( mode ) {
  switch ( mode ) {
    case LexerDriverMode::FILE: {
      source_stream_ = std::make_unique<std::ifstream>( std::move( source ) );
      if ( ( static_cast<std::ifstream &>( *source_stream_ ) ).is_open() ) {
        lexer_ = std::make_unique<Lexer>( *source_stream_ );
      } else {
        std::cerr << "Unable to open file: " << source << std::endl;
        std::exit( 1 );
      }
      break;
    }
    case LexerDriverMode::CIN: {
      lexer_ = std::make_unique<Lexer>( std::cin );
      break;
    }
    case LexerDriverMode::ARG: {
      source_stream_ = std::make_unique<std::stringstream>( std::move( source ) );
      lexer_ = std::make_unique<Lexer>( *source_stream_ );
      break;
    }
  }
  if ( !lexer_ ) {
    std::exit( 1 );
  }
}

void LexerDriver::runOnFileInput() {
  while ( true ) {
    try {
      Token t = lexer_->getNextToken();
      std::cout << t;
      if ( t.type_ == TokenType::END_OF_FILE ) break;
    } catch ( const LexerException &e ) {
      std::cout << e.what();
      std::cout << "Terminating after error." << std::endl;
      std::exit( 1 );
    } catch ( const std::exception &e ) {
      std::cout << "DEBUG: unexpected system exception: " << e.what();
      std::exit( 1 );
    }
  }
  std::cout << "Whole file read. Exiting." << std::endl;
  std::exit( 0 );
}

void LexerDriver::runOnCinInput() {
  while ( true ) {
    std::cout << "Rusthon++ > " << std::flush;
    try {
      while ( true ) {
        Token t = lexer_->getNextToken();
        std::cout << t;
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
      std::cin.putback('\n');
      std::cout << lexer_->getNextToken();
    } catch ( const std::exception &e ) {
      std::cout << "DEBUG: unexpected system exception: " << e.what();
      std::exit( 1 );
    }
  }
}

void LexerDriver::runOnArgInput() {
  while ( true ) {
    try {
      Token t = lexer_->getNextToken();
      std::cout << t;
      if ( t.type_ == TokenType::END_OF_FILE ) break;
    } catch ( const LexerException &e ) {
      std::cout << e.what();
      std::cout << "Terminating after error." << std::endl;
      std::exit( 1 );
    } catch ( const std::exception &e ) {
      std::cout << "DEBUG: unexpected system exception: " << e.what();
      std::exit( 1 );
    }
  }
  std::cout << "Whole arg read. Exiting." << std::endl;
  std::exit( 0 );
}

void LexerDriver::run() {
  switch ( mode_ ) {
    case LexerDriverMode::FILE: runOnFileInput();
    case LexerDriverMode::CIN: runOnCinInput();
    case LexerDriverMode::ARG: runOnArgInput();
  }
}