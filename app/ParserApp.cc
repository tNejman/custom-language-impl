#include <format>
#include <iostream>
#include <sstream>

#include "ArgParser.hpp"
#include "Drivers/Formatter.hpp"
#include "Drivers/LexerDriver.h"
#include "Parser/AstPrinterVisitor.h"
#include "Parser/AstSerializerVisitor.h"
#include "Parser/Node.h"
#include "Parser/Parser.h"

int main( int argc, const char** argv ) {
  // std::stringstream ss( "if (true) do\nint x = 3\n done" );
  // std::stringstream ss( "int x = 3\nchar y = 'c'" );
  // std::stringstream ss{ "var bool c = x == 1 and y != 2" };
  std::stringstream ss{ "var int x 5" };
  Lexer lexer( ss );
  Parser parser( lexer );
  // AstPrinterVisitor printer{};
  AstSerializerVisitor serlializer{};
  auto root = parser.buildProgram();
  // root.accept( printer );
  root->accept( serlializer );
  std::cout << serlializer.getString() << std::endl;

  ArgParseLexerResult config = parseLexerArguments( argc, argv );
  auto driver = driver_factory::build( config );
  // Lexer lexer(std::cin);
  // Parser parser(lexer);
  // auto program = parser.buildProgram();
  // AstPrinterVisitor printer{};
  // program->accept(printer);
}