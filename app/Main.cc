#include <fstream>
#include <iostream>
#include <sstream>

#include "Lexer/Lexer.h"

int main() {
  std::cout << "This is main.cc file" << std::endl;

  {
    std::string code = "int x = 5";
    std::stringstream ss( code );
    Lexer lexer( ss );
  }
  {
    std::ifstream file( "source.txt" );
    if ( file.is_open() ) {
      Lexer lexer( file );
    }
  }
  {
    Lexer lexer(std::cin);
  }
}