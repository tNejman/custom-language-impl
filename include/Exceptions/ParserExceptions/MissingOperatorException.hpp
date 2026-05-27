#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"
#include "Parser/ParserHelper.h"

class MissingOperatorException : public ParserException {
 private:
 public:
  MissingOperatorException( const Position pos, const TokenType op, const std::string_view object_being_built )
      : ParserException( pos, std::format( "Missing Operator '{}' when building '{}'.", magic_enum::enum_name( op ),
                                           object_being_built ) ) {
    assert( parser_helper::isOperator( op )
            && "MissingOperatorException only accepts TokenType values that represent operators" );
  }
};