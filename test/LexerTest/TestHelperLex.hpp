#pragma once

#include <gtest/gtest.h>

#include "Lexer/Lexer.h"
#include "Lexer/Token.hpp"

class LexerTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

void assertTokensTypeAndPos( std::string code, std::vector<Token> &&expected ) {
  std::stringstream ss{ code };
  Lexer lexer{ ss };
  for ( const auto &exp : expected ) {
    ASSERT_EQ( exp, lexer.getNextToken() );
  }
}

void assertTokensType( std::string &&code, std::vector<TokenType> &&expected ) {
  std::stringstream ss{ code };
  Lexer lexer{ ss };
  for ( const auto &exp : expected ) {
    ASSERT_EQ( exp, lexer.getNextToken().type_ );
  }
}

bool areFloatsKindaEqual( const float f1, const float f2 ) {
  return std::abs( f1 - f2 ) < 0.001;
}