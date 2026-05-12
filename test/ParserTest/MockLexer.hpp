#pragma once

#include <deque>
#include <vector>

#include "Lexer/ILexer.h"
#include "Lexer/Token.hpp"

class MockLexer : public ILexer {
  std::deque<Token> token_stream_;

 public:
  MockLexer( std::deque<Token> init_stream ) : token_stream_( std::move( init_stream ) ) {
  }
  MockLexer( std::vector<Token> init_stream ) : token_stream_( init_stream.begin(), init_stream.end() ) {
  }

  Token getNextToken() override {
    if ( token_stream_.size() > 0 ) {
      Token to_return = std::move( token_stream_.front() );
      token_stream_.pop_front();
      return to_return;
    }
    return Token{ Position{ 99, 99 }, TokenType::END_OF_FILE, std::monostate{} };
  }
};