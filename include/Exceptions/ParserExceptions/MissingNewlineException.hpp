#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class MissingNewlineException : public ParserException {
 private:
 public:
  MissingNewlineException( const Position pos, const std::string_view object_after_which_is_missing )
      : ParserException( pos, std::format( "Missing newline after '{}'.", object_after_which_is_missing ) ) {
  }
  MissingNewlineException( const Position pos, const TokenType token_after_which_is_missing,
                           const std::string_view in_context_of )
      : ParserException( pos, std::format( "Missing newline after '{}' in context: '{}'.",
                                           magic_enum::enum_name( token_after_which_is_missing ), in_context_of ) ) {
  }
};