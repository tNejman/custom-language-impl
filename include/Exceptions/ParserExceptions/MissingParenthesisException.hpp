#pragma once

#include <string_view>

#include "Exceptions/ParserExceptions/_ParserException.hpp"

enum class ParenthesisType { OPENING, CLOSING };

inline std::string_view parenthesisTypeEnumToString( ParenthesisType par_type ) {
  return par_type == ParenthesisType::OPENING ? "opening" : "closing";
}

class MissingParenthesisException : public ParserException {
 private:
 public:
  MissingParenthesisException( const Position pos, const std::string_view object_built_at_the_time,
                               ParenthesisType par_type )
      : ParserException( pos, std::format( "Missing {} parenthesis when building '{}'.",
                                           parenthesisTypeEnumToString( par_type ), object_built_at_the_time ) ) {
  }
};