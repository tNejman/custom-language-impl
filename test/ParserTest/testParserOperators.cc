#include <gtest/gtest.h>

#include "Lexer/Token.hpp"
#include "TestHelperPars.hpp"

class ParserOperatorsTest : public ParserTest {};

// TEST_F(ParserOperatorPriorityTest, ) {
//   std::vector<TokenInitializer> init = {

//   };
//   std::string res = initTokensBuildProgramAndSerialize(std::move(init));
// }

TEST_F( ParserOperatorsTest, mult_before_add ) {
  std::vector<TokenInitializer> init = {
      { TokenType::KW_VAR },         { TokenType::T_INT },   { TokenType::IDENTIFIER, "x" }, { TokenType::OP_ASSIGN },
      { TokenType::INT_LITERAL, 1 }, { TokenType::OP_PLUS }, { TokenType::INT_LITERAL, 2 },  { TokenType::OP_MUL },
      { TokenType::INT_LITERAL, 3 }, { TokenType::NEWLINE }, { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var INT x = {1 + {2 * 3}}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, subtraction_left_to_right ) {
  std::vector<TokenInitializer> init = {
      { TokenType::KW_VAR },          { TokenType::T_INT },    { TokenType::IDENTIFIER, "x" }, { TokenType::OP_ASSIGN },
      { TokenType::INT_LITERAL, 10 }, { TokenType::OP_MINUS }, { TokenType::INT_LITERAL, 5 },  { TokenType::OP_MINUS },
      { TokenType::INT_LITERAL, 2 },  { TokenType::NEWLINE },  { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var INT x = {{10 - 5} - 2}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, logical_or_precedence ) {
  std::vector<TokenInitializer> init = {
      { TokenType::KW_VAR },          { TokenType::T_BOOL }, { TokenType::IDENTIFIER, "b" }, { TokenType::OP_ASSIGN },
      { TokenType::IDENTIFIER, "x" }, { TokenType::OP_EQ },  { TokenType::INT_LITERAL, 1 },  { TokenType::OP_AND },
      { TokenType::IDENTIFIER, "y" }, { TokenType::OP_NEQ }, { TokenType::INT_LITERAL, 2 },  { TokenType::NEWLINE },
      { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var BOOL b = {{x == 1} and {y != 2}}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, array_map_chain ) {
  std::vector<TokenInitializer> init = { { TokenType::T_INT },     { TokenType::LBRACKET },
                                         { TokenType::RBRACKET },  { TokenType::IDENTIFIER, "val" },
                                         { TokenType::OP_ASSIGN }, { TokenType::IDENTIFIER, "arr" },
                                         { TokenType::OP_MAP },    { TokenType::IDENTIFIER, "foo1" },
                                         { TokenType::OP_MAP },    { TokenType::IDENTIFIER, "foo2" },
                                         { TokenType::NEWLINE },   { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{INT[] val = {{arr -> foo1} -> foo2}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, cast_precedence ) {
  std::vector<TokenInitializer> init = {
      { TokenType::KW_VAR },     { TokenType::T_FLOAT },  { TokenType::IDENTIFIER, "f" },
      { TokenType::OP_ASSIGN },  { TokenType::OP_MINUS }, { TokenType::IDENTIFIER, "x" },
      { TokenType::KW_CAST_TO }, { TokenType::T_FLOAT },  { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var FLOAT f = {-{x cast_to FLOAT}}}}", std::move( res ) );
}