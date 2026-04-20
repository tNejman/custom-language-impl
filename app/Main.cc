#include <iomanip>
#include <iostream>
#include <sstream>

#include "LexerDriver.h"

void assertTokensType( const std::string &&code, const std::vector<TokenType> &&expected ) {
  std::stringstream ss{ code };
  Lexer lexer{ ss };
  for ( const auto &exp : expected ) {
    std::cout << "Excpected: " << std::left << std::setw( 12 ) << exp << "Actual: " << lexer.getNextToken().type_
              << std::endl;
  }
}

int main( int argc, const char **argv ) {
  // if ( argc == 1 ) {
  //   LexerDriver ld{ LexerDriverMode::CIN, "" };
  // } else if ( argc == 3 && argv[1] == "-m" && argv[2] == "cin" ) {
  //   LexerDriver
  // }
}