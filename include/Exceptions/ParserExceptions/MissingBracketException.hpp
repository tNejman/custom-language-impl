#pragma once

#include <string_view>

#include "Exceptions/ParserExceptions/_ParserException.hpp"

enum class BracketType { OPENING, CLOSING };

inline std::string_view bracketTypeEnumToString( BracketType par_type ) {
  return par_type == BracketType::OPENING ? "opening" : "closing";
}

class MissingBracketException : public ParserException {
 private:
  const BracketType bracket_type_;

 public:
  MissingBracketException( const Position pos, const std::string_view object_built_at_the_time, BracketType brack_type )
      : ParserException( pos, std::format( "Missing {} bracket when building '{}'.",
                                           bracketTypeEnumToString( brack_type ), object_built_at_the_time ) ),
        bracket_type_( ( brack_type ) ) {
  }
  BracketType getBracketType() const noexcept {
    return bracket_type_;
  }
};