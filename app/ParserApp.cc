#include <format>
#include <iostream>
#include <sstream>

#include "Drivers/Formatter.hpp"
#include "Parser/AstPrinterVisitor.h"
#include "Parser/AstSerializerVisitor.h"
#include "Parser/Node.h"
#include "Parser/Parser.h"

int main() {
  std::stringstream ss( "if (true) do\nint x = 3\n done" );
  // std::stringstream ss( "int x = 3\nchar y = 'c'" );
  Lexer lexer( ss );
  Parser parser( lexer );
  // AstPrinterVisitor printer{};
  AstSerializerVisitor serlializer{};
  auto root = parser.buildProgram();
  // root.accept( printer );
  root.accept( serlializer );
  std::cout << serlializer.getString() << std::endl;
}