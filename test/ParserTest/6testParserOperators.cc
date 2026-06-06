#include <gtest/gtest.h>

#include "Lexer/Token.hpp"
#include "TestHelperPars.hpp"

class ParserOperatorsTest : public ParserTest {};

TEST_F( ParserOperatorsTest, assignment ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x = y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, addition_assignment ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_ADD_ASSIGN, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x += y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, subtraction_assignment ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_SUB_ASSIGN, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x -= y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplication_assignment ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_MUL_ASSIGN, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x *= y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, division_assignment ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_DIV_ASSIGN, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x /= y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, modulo_assignment ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_MOD_ASSIGN, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x %= y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, logical_or ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_OR, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x or y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, logical_and ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_AND, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x and y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, equality ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_EQ, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x == y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, inequality ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_NEQ, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x != y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, relational_less_than ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_LT, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x < y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, relational_less_or_equal ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_LEQ, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x <= y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, relational_greater_than ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_GT, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x > y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, relational_greater_or_equal ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_GEQ, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x >= y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_addition ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_PLUS, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x + y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_subtraction ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_MINUS, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x - y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_concatenation ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_CONCAT, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x ++ y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_diff ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_DIFF, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x -- y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplicative_multiplication ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_MUL, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x * y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplicative_division ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_DIV, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x / y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplicative_modulo ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_MOD, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x % y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, unary_negation_arithmetical ) {
  std::vector<TokenInitializer> init = { TokenType::OP_MINUS, { TokenType::IDENTIFIER, "x" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{-x}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, unary_negation_logical ) {
  std::vector<TokenInitializer> init = { TokenType::OP_NOT, { TokenType::IDENTIFIER, "x" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{not x}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, unary_reverse ) {
  std::vector<TokenInitializer> init = { TokenType::OP_REV, { TokenType::IDENTIFIER, "x" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{@x}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, unary_len ) {
  std::vector<TokenInitializer> init = { TokenType::OP_LEN, { TokenType::IDENTIFIER, "x" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{$x}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, cast_to ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::KW_CAST_TO, TokenType::T_INT, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x cast_to INT}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, access_index ) {
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" },
                                         TokenType::LBRACKET,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RBRACKET,
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x[y]}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, access_map ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_MAP, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x MAP y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, access_filter ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "x" }, TokenType::OP_FILTER, { TokenType::IDENTIFIER, "y" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x FILTER y}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, logical_before_assignment ) {
  // x = y or z
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN,
                                         { TokenType::IDENTIFIER, "y" }, TokenType::OP_OR,
                                         { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x = {y or z}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, logical_before_assignment_overriden_by_parentheses ) {
  // (x = y) or z
  std::vector<TokenInitializer> init = { TokenType::LPAREN,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RPAREN,
                                         TokenType::OP_OR,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x = y} or z}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, and_before_or ) {
  // x or y and z
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" }, TokenType::OP_OR,
                                         { TokenType::IDENTIFIER, "y" }, TokenType::OP_AND,
                                         { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x or {y and z}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, and_before_or_overriden_by_parentheses ) {
  // (x or y) and z
  std::vector<TokenInitializer> init = { TokenType::LPAREN,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_OR,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RPAREN,
                                         TokenType::OP_AND,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x or y} and z}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, equality_before_and ) {
  // x and y == z
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" }, TokenType::OP_AND,
                                         { TokenType::IDENTIFIER, "y" }, TokenType::OP_EQ,
                                         { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x and {y == z}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, equality_before_and_overriden_by_parentheses ) {
  // (x and y) == z
  std::vector<TokenInitializer> init = { TokenType::LPAREN,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_AND,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RPAREN,
                                         TokenType::OP_EQ,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x and y} == z}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, reliative_before_equality ) {
  // x == y < z
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" }, TokenType::OP_EQ,
                                         { TokenType::IDENTIFIER, "y" }, TokenType::OP_LT,
                                         { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x == {y < z}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, reliative_before_equality_overriden_by_parentheses ) {
  // (x == y) < z
  std::vector<TokenInitializer> init = { TokenType::LPAREN,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_EQ,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RPAREN,
                                         TokenType::OP_LT,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x == y} < z}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_before_relative ) {
  // x > y + z
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" }, TokenType::OP_GT,
                                         { TokenType::IDENTIFIER, "y" }, TokenType::OP_PLUS,
                                         { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x > {y + z}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_before_relative_overriden_by_parentheses ) {
  // (x > y) + z
  std::vector<TokenInitializer> init = { TokenType::LPAREN,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_GT,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RPAREN,
                                         TokenType::OP_PLUS,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x > y} + z}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplicative_before_additive ) {
  // x + y * z
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" }, { TokenType::OP_PLUS },
                                         { TokenType::IDENTIFIER, "y" }, { TokenType::OP_MUL },
                                         { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x + {y * z}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplicative_before_additive_overriden_by_parentheses ) {
  // (x + y) * z
  std::vector<TokenInitializer> init = {
      TokenType::LPAREN, { TokenType::IDENTIFIER, "x" }, { TokenType::OP_PLUS },         { TokenType::IDENTIFIER, "y" },
      TokenType::RPAREN, { TokenType::OP_MUL },          { TokenType::IDENTIFIER, "z" }, TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x + y} * z}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, unary_before_multiplicative ) {
  // x * -y
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" },
                                         { TokenType::OP_MUL },
                                         { TokenType::OP_MINUS },
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{x * {-y}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, unary_before_multiplicative_overriden_by_parentheses ) {
  // -(x * y)
  std::vector<TokenInitializer> init = { TokenType::OP_MINUS,
                                         TokenType::LPAREN,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_MUL,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RPAREN,
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{-{x * y}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, cast_before_unary ) {
  // -x cast_to float
  std::vector<TokenInitializer> init = { { TokenType::OP_MINUS },
                                         { TokenType::IDENTIFIER, "x" },
                                         { TokenType::KW_CAST_TO },
                                         { TokenType::T_FLOAT },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{-{x cast_to FLOAT}}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, cast_before_unary_overriden_by_parentheses ) {
  // (-x) cast_to float
  std::vector<TokenInitializer> init = { TokenType::LPAREN, { TokenType::OP_MINUS },   { TokenType::IDENTIFIER, "x" },
                                         TokenType::RPAREN, { TokenType::KW_CAST_TO }, { TokenType::T_FLOAT },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{-x} cast_to FLOAT}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, access_before_cast ) {
  // x[y] cast_to int
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" },
                                         TokenType::LBRACKET,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RBRACKET,
                                         TokenType::KW_CAST_TO,
                                         TokenType::T_INT,
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x[y]} cast_to INT}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, operaor_map_chain ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "arr" },  { TokenType::OP_MAP },
      { TokenType::IDENTIFIER, "foo1" }, { TokenType::OP_MAP },
      { TokenType::IDENTIFIER, "foo2" }, { TokenType::NEWLINE },
  };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{arr MAP foo1} MAP foo2}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, operaor_filter_chain ) {
  std::vector<TokenInitializer> init = {
      { TokenType::IDENTIFIER, "arr" }, { TokenType::OP_FILTER },          { TokenType::IDENTIFIER, "foo1" },
      { TokenType::OP_FILTER },         { TokenType::IDENTIFIER, "foo2" }, { TokenType::NEWLINE },
  };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{arr FILTER foo1} FILTER foo2}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, operator_access_chain ) {
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" },
                                         TokenType::LBRACKET,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RBRACKET,
                                         TokenType::LBRACKET,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::RBRACKET,
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{x[y]}[z]}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, mixed_access_operator_chain ) {
  std::vector<TokenInitializer> init = { { TokenType::IDENTIFIER, "x" },
                                         TokenType::LBRACKET,
                                         { TokenType::IDENTIFIER, "y" },
                                         TokenType::RBRACKET,
                                         TokenType::OP_MAP,
                                         { TokenType::IDENTIFIER, "z" },
                                         TokenType::OP_FILTER,
                                         { TokenType::IDENTIFIER, "k" },
                                         TokenType::NEWLINE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{{x[y]} MAP z} FILTER k}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, additive_left_associativity ) {
  std::vector<TokenInitializer> init = { { TokenType::INT_LITERAL, 10 }, { TokenType::OP_PLUS },
                                         { TokenType::INT_LITERAL, 5 },  { TokenType::OP_PLUS },
                                         { TokenType::INT_LITERAL, 2 },  { TokenType::NEWLINE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{10 + 5} + 2}}", std::move( res ) );
}

TEST_F( ParserOperatorsTest, multiplicative_left_associativity ) {
  std::vector<TokenInitializer> init = { { TokenType::INT_LITERAL, 10 }, { TokenType::OP_MUL },
                                         { TokenType::INT_LITERAL, 5 },  { TokenType::OP_MUL },
                                         { TokenType::INT_LITERAL, 2 },  { TokenType::NEWLINE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{{10 * 5} * 2}}", std::move( res ) );
}
