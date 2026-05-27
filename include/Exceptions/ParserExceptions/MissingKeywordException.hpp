#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"
#include "magic_enum/magic_enum.hpp"

class MissingKeywordException : public ParserException {
 private:
  const TokenType keyword_;

 public:
  MissingKeywordException( const Position pos, const TokenType keyword, const std::string_view object_being_built )
      : ParserException( pos, std::format( "Missing Keyword '{}' when building '{}'.", magic_enum::enum_name( keyword ),
                                           object_being_built ) ),
        keyword_( keyword ) {
    assert( magic_enum::enum_name( keyword ).starts_with( "KW_" ) );
  }
  TokenType getTokenTypeKeyword() const noexcept {
    return keyword_;
  }
};