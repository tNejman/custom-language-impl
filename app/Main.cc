#include <fstream>
#include <iomanip>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <sstream>

#include "Lexer/Lexer.h"

void assertTokensType( const std::string&& code, const std::vector<TokenType>&& expected ) {
  std::stringstream ss{ code };
  Lexer lexer{ ss };
  for ( const auto& exp : expected ) {
    std::cout << "Excpected: " << std::left << std::setw( 12 ) << exp << "Actual: " << lexer.getNextToken().type_
              << std::endl;
  }
}

int main() {
  // std::cout << "This is main.cc file" << std::endl;

  // {
  //   std::string code = "int x = 5";
  //   std::stringstream ss( code );
  //   Lexer lexer( ss );
  // }
  // {
  //   std::ifstream file( "source.txt" );
  //   if ( file.is_open() ) {
  //     Lexer lexer( file );
  //   }
  // }
  // {
  //   Lexer lexer( std::cin );
  // }

  assertTokensType(
      R"end(if (a >= 10 and b <= 20) do
    a += 1
done elseif (a != b or not c) do
    b -= 1
done else do
    a = 2
    b /= 2
done)end",
      { TokenType::KW_DEF, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::LPAREN, TokenType::T_INT,
        TokenType::IDENTIFIER, TokenType::COMMA, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::RPAREN,
        TokenType::KW_DO, TokenType::NEWLINE,

        TokenType::KW_RET, TokenType::IDENTIFIER, TokenType::OP_PLUS, TokenType::IDENTIFIER, TokenType::NEWLINE,

        TokenType::KW_DONE, TokenType::END_OF_FILE } );
}