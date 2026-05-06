#include <format>
#include <iostream>
#include <sstream>

#include "Drivers/Formatter.hpp"
#include "Parser/AstPrinterVisitor.h"
#include "Parser/Node.h"
#include "Parser/Parser.h"

int main() {
  std::stringstream ss( "[int] x = [1]" );
  Lexer lexer( ss );
  Parser parser( lexer );
  AstPrinterVisitor printer{};
  auto root = parser.buildProgram();
  root.accept( printer );
}