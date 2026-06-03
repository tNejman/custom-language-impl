#pragma once

#include <gtest/gtest.h>

#include "MockParser.hpp"

class InterpreterTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

// struct TokenInitializer {
//   TokenType type_;
//   TokenVal val_;

//   TokenInitializer( TokenType type, TokenVal val ) : type_( type ), val_( std::move( val ) ) {
//   }
//   TokenInitializer( TokenType type ) : type_( type ), val_( std::monostate{} ) {
//   }
// };

// std::deque<Token> makeTokens( std::vector<TokenInitializer> init_list ) {
//   std::deque<Token> deque{};
//   Position cur_pos{ 1, 1 };
//   for ( auto init : init_list ) {
//     deque.push_back( Token{ cur_pos, init.type_, std::move( init.val_ ) } );
//     cur_pos = Position{ cur_pos.line_, cur_pos.column_ + 1 };
//   }
//   return deque;
// }

// std::unique_ptr<ProgramNode> initTokensAndBuildProgram( std::vector<TokenInitializer> init_list ) {
//   MockLexer mock_lexer{ makeTokens( std::move( init_list ) ) };
//   Parser parser{ mock_lexer };
//   return parser.buildProgram();
// }

// std::string initTokensBuildProgramAndSerialize( std::vector<TokenInitializer> init_vec ) {
//   std::unique_ptr<ProgramNode> program_ptr = initTokensAndBuildProgram( std::move( init_vec ) );
//   AstSerializerVisitor serializer{};
//   program_ptr->accept( serializer );
//   return serializer.getString();
// }

// bool isSameTokenTypeAsBaseType( TokenType tt, BaseType bt ) {
//   switch ( tt ) {
//     case TokenType::T_BOOL: return bt == BaseType::BOOL;
//     case TokenType::T_CHAR: return bt == BaseType::CHAR;
//     case TokenType::T_FLOAT: return bt == BaseType::FLOAT;
//     case TokenType::T_INT: return bt == BaseType::INT;
//     default: throw std::runtime_error( "type error" );
//   }
// }