#include <fstream>
#include <memory>
#include <sstream>

#include "ArgParser.hpp"
#include "Drivers/LexerDriver.h"

int main( int argc, const char** argv ) {
  ArgParseLexerResult config = parseLexerArguments( argc, argv );
  auto driver = driver_factory::build( config );
  driver->run();

  return 0;
}