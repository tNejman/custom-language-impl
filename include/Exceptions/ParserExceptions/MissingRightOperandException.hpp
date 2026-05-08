#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"
#include "Parser/Node.h"
#include "Parser/ParserHelper.h"

class MissingRightOperandException : public ParserException {
 private:
 public:
  MissingRightOperandException( const Position pos, AssignmentType assignment_type )
      : ParserException( pos, std::format( "Missing Right Operand after operator '{}':'{}'",
                                           magic_enum::enum_name( assignment_type ),
                                           parser_helper::operatorToStr( assignment_type ) ) ) {
  }
  MissingRightOperandException( const Position pos, BinaryOperator binary_operator )
      : ParserException( pos, std::format( "Missing Right Operand after operator '{}':'{}'",
                                           magic_enum::enum_name( binary_operator ),
                                           parser_helper::operatorToStr( binary_operator ) ) ) {
  }
  MissingRightOperandException( const Position pos, UnaryOperator unary_operator )
      : ParserException(
            pos, std::format( "Missing Right Operand after operator '{}':'{}'", magic_enum::enum_name( unary_operator ),
                              parser_helper::operatorToStr( unary_operator ) ) ) {
  }
  MissingRightOperandException( const Position pos, const std::string_view access_operator )
      : ParserException( pos, std::format( "Missing Right Operand after operator 'ACCESS':'[]'" ) ) {
    assert( access_operator == "[]" );
  }
};