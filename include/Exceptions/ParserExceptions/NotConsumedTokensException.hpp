#pragma once

#include "Exceptions/ParserExceptions/_ParserException.hpp"

class NotConsumedTokensException : public ParserException {
 private:
 public:
  NotConsumedTokensException( const Position pos, const Token token_supposed_to_be_eof )
      : ParserException( pos, std::format( "Finished parsing but not reached EOF. Instead token is {}",
                                           token_supposed_to_be_eof ) ) {
  }
};