#include <gtest/gtest.h>

#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "TestHelperPars.hpp"

TEST_F( ParserTest, check_init2 ) {
  ASSERT_TRUE( true );
}

TEST_F( ParserTest, empty_program ) {
  auto program_ptr = initTokensAndBuildProgram( {} );
  ASSERT_NE( nullptr, program_ptr );
  ASSERT_EQ( 0, program_ptr->getStatementList().size() );
}

TEST_F( ParserTest, one_statement_without_newline ) {
  std::unique_ptr<ProgramNode> program{};
  ASSERT_NO_THROW(
      program = initTokensAndBuildProgram(
          { TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 1 } } ) );
  ASSERT_NE( nullptr, program );
}

TEST_F( ParserTest, multiple_newlines_at_the_begining ) {
  std::vector<TokenInitializer> init = { TokenType::NEWLINE,
                                         TokenType::NEWLINE,
                                         TokenType::NEWLINE,
                                         TokenType::T_INT,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::INT_LITERAL, 1 } };
  std::string res;
  ASSERT_NO_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ) );
  EXPECT_EQ( "{{INT x = 1}}", std::move( res ) );
}

TEST_F( ParserTest, multiple_statements_in_same_line ) {
  ASSERT_THROW( initTokensAndBuildProgram( {
                    TokenType::T_INT,
                    { TokenType::IDENTIFIER, "x" },
                    TokenType::OP_ASSIGN,
                    { TokenType::INT_LITERAL, 1 },
                    TokenType::T_INT,
                    { TokenType::IDENTIFIER, "y" },
                    TokenType::OP_ASSIGN,
                    { TokenType::INT_LITERAL, 2 },
                } ),
                NotConsumedTokensException );
}

/* Statement productions at top level
 */

TEST_F( ParserTest, statement_while ) {
  std::vector<TokenInitializer> init = { TokenType::KW_WHILE, TokenType::LPAREN,  { TokenType::BOOL_LITERAL, true },
                                         TokenType::RPAREN,   TokenType::KW_DO,   TokenType::NEWLINE,
                                         TokenType::KW_DONE,  TokenType::NEWLINE, TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{while (true) {}}}", std::move( res ) );
}

TEST_F( ParserTest, statement_loop_control_break ) {
  std::vector<TokenInitializer> init = { TokenType::KW_BREAK, TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{break}}", std::move( res ) );
}

TEST_F( ParserTest, statement_loop_control_continue ) {
  std::vector<TokenInitializer> init = { TokenType::KW_CONTINUE, TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{continue}}", std::move( res ) );
}

TEST_F( ParserTest, statement_return ) {
  std::vector<TokenInitializer> init = { TokenType::KW_RET, TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{ret}}", std::move( res ) );
}

/* Advanced statement productions
 */



/*



*/

TEST_F( ParserTest, array_literal_empty ) {
  std::vector<TokenInitializer> init = { TokenType::LBRACKET, TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_single_element ) {
  std::vector<TokenInitializer> init = { TokenType::LBRACKET, { TokenType::INT_LITERAL, 1 }, TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[1]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_many_elements ) {
  std::vector<TokenInitializer> init = { TokenType::LBRACKET,
                                         { TokenType::INT_LITERAL, 1 },
                                         TokenType::COMMA,
                                         { TokenType::INT_LITERAL, 2 },
                                         TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[1, 2]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_very_many_elements ) {
  std::vector<TokenInitializer> init = {
      TokenType::LBRACKET, { TokenType::INT_LITERAL, 1 },  TokenType::COMMA, { TokenType::INT_LITERAL, 2 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 3 },  TokenType::COMMA, { TokenType::INT_LITERAL, 4 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 5 },  TokenType::COMMA, { TokenType::INT_LITERAL, 6 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 7 },  TokenType::COMMA, { TokenType::INT_LITERAL, 8 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 9 },  TokenType::COMMA, { TokenType::INT_LITERAL, 10 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 11 }, TokenType::COMMA, { TokenType::INT_LITERAL, 12 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 13 }, TokenType::COMMA, { TokenType::INT_LITERAL, 14 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 15 }, TokenType::COMMA, { TokenType::INT_LITERAL, 16 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 17 }, TokenType::COMMA, { TokenType::INT_LITERAL, 18 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 19 }, TokenType::COMMA, { TokenType::INT_LITERAL, 20 },
      TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_nested_many_elements ) {
  std::vector<TokenInitializer> init = {
      TokenType::LBRACKET, TokenType::LBRACKET,           { TokenType::INT_LITERAL, 1 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 2 }, TokenType::RBRACKET,
      TokenType::COMMA,    TokenType::LBRACKET,           { TokenType::INT_LITERAL, 3 },
      TokenType::COMMA,    { TokenType::INT_LITERAL, 4 }, TokenType::RBRACKET,
      TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[[1, 2], [3, 4]]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_nested_many_times ) {
  std::vector<TokenInitializer> init = {
      TokenType::LBRACKET, TokenType::LBRACKET, TokenType::LBRACKET, TokenType::LBRACKET, TokenType::LBRACKET,
      TokenType::LBRACKET, TokenType::LBRACKET, TokenType::LBRACKET, TokenType::LBRACKET, TokenType::LBRACKET,
      TokenType::RBRACKET, TokenType::RBRACKET, TokenType::RBRACKET, TokenType::RBRACKET, TokenType::RBRACKET,
      TokenType::RBRACKET, TokenType::RBRACKET, TokenType::RBRACKET, TokenType::RBRACKET, TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[[[[[[[[[[]]]]]]]]]]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_wrapped_in_parenthesis ) {
  std::vector<TokenInitializer> init = {
      TokenType::LPAREN, TokenType::LBRACKET, { TokenType::INT_LITERAL, 1 }, TokenType::RBRACKET, TokenType::RPAREN };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[1]}", std::move( res ) );
}

TEST_F( ParserTest, array_literal_expression_as_value ) {
  // [getValue(), 1 + 2]
  std::vector<TokenInitializer> init = {
      TokenType::LBRACKET, { TokenType::IDENTIFIER, "getValue" }, TokenType::LPAREN,  TokenType::RPAREN,
      TokenType::COMMA,    { TokenType::INT_LITERAL, 1 },         TokenType::OP_PLUS, { TokenType::INT_LITERAL, 2 },
      TokenType::RBRACKET };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{[getValue(), {1 + 2}]}", std::move( res ) );
}