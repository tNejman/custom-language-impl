#include "Lexer/Lexer.h"

void Lexer::nextChar() {
  if ( input_stream_.get( current_char_ ) ) {
    // update pos
  } else {
    current_char_ = '\0';
  }
}

Lexer::Lexer( std::istream& input ) : input_stream_( input ) {
  nextChar();
}

Token Lexer::getNextToken() {
  throw std::exception();
}