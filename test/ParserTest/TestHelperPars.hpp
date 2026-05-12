#pragma once

#include <gtest/gtest.h>

#include "Lexer/Token.hpp"
#include "MockLexer.hpp"
#include "Parser/AstSerializerVisitor.h"
#include "Parser/Node.h"
#include "Parser/Parser.h"

class ParserTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

struct TokenInitializer {
  TokenType type_;
  TokenVal val_;

  TokenInitializer( TokenType type, TokenVal val ) : type_( type ), val_( std::move( val ) ) {
  }
  TokenInitializer( TokenType type ) : type_( type ), val_( std::monostate{} ) {
  }
};

std::deque<Token> makeTokens( std::vector<TokenInitializer> init_list ) {
  std::deque<Token> deque{};
  Position cur_pos{ 1, 1 };
  for ( auto init : init_list ) {
    deque.push_back( Token{ cur_pos, init.type_, std::move( init.val_ ) } );
    cur_pos = Position{ cur_pos.line_, cur_pos.column_ + 1 };
  }
  return deque;
}

std::unique_ptr<ProgramNode> initTokensAndBuildProgram( std::vector<TokenInitializer> init_list ) {
  MockLexer mock_lexer{ makeTokens( std::move( init_list ) ) };
  Parser parser{ mock_lexer };
  return parser.buildProgram();
}

std::string initTokensBuildProgramAndSerialize( std::vector<TokenInitializer> init_vec ) {
  std::unique_ptr<ProgramNode> program_ptr = initTokensAndBuildProgram( std::move( init_vec ) );
  AstSerializerVisitor serializer{};
  program_ptr->accept( serializer );
  return serializer.getString();
}

// void assertTokensTypeAndPos( std::string code, std::vector<Token> &&expected ) {
//   std::stringstream ss{ code };
//   Lexer lexer{ ss };
//   for ( const auto &exp : expected ) {
//     ASSERT_EQ( exp, lexer.getNextToken() );
//   }
// }

// void assertTokensType( std::string &&code, std::vector<TokenType> &&expected ) {
//   std::stringstream ss{ code };
//   Lexer lexer{ ss };
//   for ( const auto &exp : expected ) {
//     ASSERT_EQ( exp, lexer.getNextToken().type_ );
//   }
// }

// bool areFloatsKindaEqual( const float f1, const float f2 ) {
//   return std::abs( f1 - f2 ) < 0.001;
// }