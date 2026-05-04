#pragma once

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "argparse/argparse.hpp"

enum class LexerInputSource { FILE, CIN, ARG };

struct ArgParseLexerResult {
  LexerInputSource mode = LexerInputSource::CIN;
  std::string str_argument{};
  std::filesystem::path filepath{};
};

std::string getLexerHelpMsg( const char* prog_name ) {
  static const std::string lexer_help_msg = std::format(
      "Usage: {} [OPTIONS]\n\n"
      "Options:\n"
      "  -h, --help       Show this help message and exit.\n"
      "  -m <mode>        Specify the input mode. Valid modes are:\n"
      "                     cin  : Read from standard input (default).\n"
      "                     file : Read from a file. Requires a <filename>.\n"
      "                     arg  : Read from remaining command line arguments.\n\n",
      prog_name );
  return lexer_help_msg;
}

ArgParseLexerResult parseLexerArguments( int argc, const char** argv ) {
  argparse::ArgumentParser program( "LexerApp" );
  program.add_argument( "-m", "--mode" )
      .help( getLexerHelpMsg( argv[0] ) )
      .default_value( std::string( "cin" ) )
      .action( [&argv]( const std::string& val ) {
        if ( val != "cin" && val != "file" && val != "arg" ) {
          std::cout << "Invalid mode.\n" << getLexerHelpMsg( argv[0] );
          std::exit( 1 );
        }
        return val;
      } );

  program.add_argument( "remaining_args" ).remaining();

  try {
    program.parse_args( argc, argv );
  } catch ( const std::exception& e ) {
    std::cerr << "Error: " << e.what() << "\n\n";
    std::cerr << program;
    std::exit( 1 );
  }

  ArgParseLexerResult result;
  std::string mode_str = program.get<std::string>( "-m" );
  std::vector<std::string> rem_args;
  try {
    rem_args = program.get<std::vector<std::string>>( "remaining_args" );
  } catch ( const std::logic_error& ) {
  }

  if ( mode_str == "cin" ) {
    result.mode = LexerInputSource::CIN;
  }

  else if ( mode_str == "file" ) {
    result.mode = LexerInputSource::FILE;
    if ( rem_args.empty() ) {
      std::cerr << "Error: '-m <file> requires a filename argument.\n\n";
      std::cerr << program;
      std::exit( 1 );
    }
    result.filepath = rem_args.front();
  }

  else if ( mode_str == "arg" ) {
    result.mode = LexerInputSource::ARG;
    for ( size_t i = 0; i < rem_args.size(); ++i ) {
      if ( i > 0 ) result.str_argument += " ";
      result.str_argument += rem_args[i];
    }
  }
  return result;
}