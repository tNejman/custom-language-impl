#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class MissingKeywordException : public ParserException {
 private:
 public:
  MissingKeywordException( const Position pos, const std::string_view keyword,
                           const std::string_view object_being_built )
      : ParserException( pos, std::format( "Missing Keyword '{}' when building '{}'.", keyword, object_being_built ) ) {
  }
};