// #include <fstream>
#include <iomanip>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <sstream>

#include "Lexer/Lexer.h"

void assertTokensType( const std::string &&code, const std::vector<TokenType> &&expected ) {
  std::stringstream ss{ code };
  Lexer lexer{ ss };
  for ( const auto &exp : expected ) {
    std::cout << "Excpected: " << std::left << std::setw( 12 ) << exp << "Actual: " << lexer.getNextToken().type_
              << std::endl;
  }
}

int main() {
  // std::cout << "This is main.cc file" << std::endl;

  {
    std::string code = R"("")";
    std::stringstream ss( code );
    Lexer lexer( ss );
    lexer.getNextToken();
  }
  // {
  //   std::ifstream file( "source.txt" );
  //   if ( file.is_open() ) {
  //     Lexer lexer( file );
  //   }
  // }
  // {
  //   Lexer lexer( std::cin );
  //   while ( true ) {
  //     std::cout << lexer.getNextToken() << std::endl;
  //   }
  // }

  // assertTokensType("12.34.56", {
  //                                  TokenType::FLOAT_LITERAL,
  //                                  TokenType::END_OF_FILE,
  //                                  TokenType::END_OF_FILE,
  //                                  TokenType::END_OF_FILE,
  //                                  TokenType::END_OF_FILE,
  //                              });

  // assertTokensType( "var counter = 0",
  //                   { TokenType::KW_VAR, TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::INT_LITERAL } );
}