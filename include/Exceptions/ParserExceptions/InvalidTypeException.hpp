#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Variable.h"

std::string_view paramQuantifierToString( Mutability mut ) {
  return "mut";
}
std::string_view paramQuantifierToString( PassMode pass_mode ) {
  return "copy";
}

class InvalidTypeException : public ParserException {
 private:
 public:
  InvalidTypeException( const Position pos ) : ParserException( pos, "Couldn't construct type at given position." ) {
  }
  InvalidTypeException( const Position pos, const std::string_view object_being_built )
      : ParserException(
            pos, std::format( "Couldn't construct type at given position, while building '{}'", object_being_built ) ) {
  }
  InvalidTypeException( const Position pos, PassMode pass_mode )
      : ParserException( pos, std::format( "Couldn't construct type at given position, after given quantifier '{}'",
                                           paramQuantifierToString( pass_mode ) ) ) {
  }
  InvalidTypeException( const Position pos, Mutability mut )
      : ParserException( pos, std::format( "Couldn't construct type at given position, after given quantifier '{}'",
                                           paramQuantifierToString( mut ) ) ) {
  }
};