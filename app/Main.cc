#include <fstream>
#include <iostream>

#include "Exceptions/InterpreterExceptions/_InterpreterException.hpp"
#include "Exceptions/LexerExceptions/_LexerException.hpp"
#include "Exceptions/ParserExceptions/_ParserException.hpp"
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
  } catch ( const LexerException& e ) {
    std::cerr << e.what() << "\n";
    return 1;
  } catch ( const ParserException& e ) {
    std::cerr << e.what() << "\n";
    return 1;
  } catch ( const InterpreterException& e ) {
    std::cerr << e.what() << "\n";
    return 1;
  } catch ( ... ) {
    std::cerr << "unknown error" << "\n";
    return 2;
  }

  return 0;
}