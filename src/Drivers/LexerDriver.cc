
#include "Drivers/LexerDriver.h"

#include <iostream>

#include "Exceptions/LexerExceptions/_LexerException.hpp"

ILexerDriver::ILexerDriver( Lexer &lexer ) : lexer_( lexer ) {
}

Lexer &ILexerDriver::getLexer() {
  return this->lexer_;
}

BatchLexerDriver::BatchLexerDriver( Lexer &lexer ) : ILexerDriver( lexer ) {
}

void BatchLexerDriver::run() {
  while ( true ) {
    try {
      Token t = getLexer().getNextToken();
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

InteractiveLexerDriver::InteractiveLexerDriver( Lexer &lexer ) : ILexerDriver( lexer ) {
}

void InteractiveLexerDriver::run() {
  while ( true ) {
    std::cout << "Rusthon++ > " << std::flush;
    try {
      while ( true ) {
        Token t = getLexer().getNextToken();
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
      std::cout << std::format( "{}", getLexer().getNextToken() );
    } catch ( const std::exception &e ) {
      std::cout << "DEBUG: unexpected system exception: " << e.what();
      std::exit( 1 );
    }
  }
}