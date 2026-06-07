#include <fstream>
#include <iostream>

#include "Interpreter/Interpreter.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

int main( int argc, char* argv[] ) {
  if ( argc != 2 ) {
    std::cerr << "Usage: " << argv[0] << " <script>\n";
    return 1;
  }

  std::ifstream file( argv[1] );
  if ( !file.is_open() ) {
    std::cerr << "Error: could not open file '" << argv[1] << "'\n";
    return 1;
  }

  try {
    Lexer lexer( file );
    Parser parser( lexer );

    auto program = parser.buildProgram();

    Interpreter interpreter( std::move( program ) );
    interpreter.execute();
  } catch ( const std::exception& e ) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}