#include <gtest/gtest.h>

#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"
#include "Lexer/Token.hpp"
#include "MockLexer.hpp"
#include "Parser/AstSerializerVisitor.h"
#include "Parser/Node.h"
#include "Parser/Parser.h"
#include "TestHelperPars.hpp"

// TEST_F(ParserTest, a) {
//   std::vector<TokenInitializer> init = {

//   };
//   std::string res = initTokensBuildProgramAndSerialize(std::move(init));
// }

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

TEST_F( ParserTest, statement_function_def ) {
  std::vector<TokenInitializer> init = { TokenType::KW_DEF,     TokenType::T_VOID,  { TokenType::IDENTIFIER, "foo" },
                                         TokenType::LPAREN,     TokenType::RPAREN,  TokenType::KW_DO,
                                         TokenType::NEWLINE,    TokenType::KW_DONE, TokenType::NEWLINE,
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{def VOID foo() {}}}", std::move( res ) );
}

TEST_F( ParserTest, statement_var_decl ) {
  std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                         TokenType::T_INT,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::INT_LITERAL, 0 },
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var INT x = 0}}", std::move( res ) );
}

TEST_F( ParserTest, statement_const_decl ) {
  std::vector<TokenInitializer> init = { TokenType::T_INT,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::INT_LITERAL, 0 },
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{INT x = 0}}", std::move( res ) );
}

TEST_F( ParserTest, statement_if ) {
  std::vector<TokenInitializer> init = { TokenType::KW_IF,   TokenType::LPAREN,  { TokenType::BOOL_LITERAL, true },
                                         TokenType::RPAREN,  TokenType::KW_DO,   TokenType::NEWLINE,
                                         TokenType::KW_DONE, TokenType::NEWLINE, TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{if (true) {} else {}}}", std::move( res ) );
}

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

/* Most basic and primary statements
 */

TEST_F( ParserTest, variable_declaration_with_basic_type ) {
  {
    std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                           TokenType::T_INT,
                                           { TokenType::IDENTIFIER, "x" },
                                           TokenType::OP_ASSIGN,
                                           { TokenType::INT_LITERAL, 1 } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var INT x = 1}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                           TokenType::T_FLOAT,
                                           { TokenType::IDENTIFIER, "x" },
                                           TokenType::OP_ASSIGN,
                                           { TokenType::FLOAT_LITERAL, 1.f } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var FLOAT x = 1.}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                           TokenType::T_BOOL,
                                           { TokenType::IDENTIFIER, "x" },
                                           TokenType::OP_ASSIGN,
                                           { TokenType::BOOL_LITERAL, true } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var BOOL x = true}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                           TokenType::T_CHAR,
                                           { TokenType::IDENTIFIER, "x" },
                                           TokenType::OP_ASSIGN,
                                           { TokenType::CHAR_LITERAL, 'c' } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var CHAR x = 'c'}}", std::move( res ) );
  }
}

TEST_F( ParserTest, constant_declaration_with_basic_type ) {
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 1 } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{INT x = 1}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_FLOAT, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::FLOAT_LITERAL, 1.f } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{FLOAT x = 1.}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_BOOL, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::BOOL_LITERAL, true } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{BOOL x = true}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_CHAR, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::CHAR_LITERAL, 'c' } };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{CHAR x = 'c'}}", std::move( res ) );
  }
}

TEST_F( ParserTest, variable_declaration_with_string_type ) {
  std::vector<TokenInitializer> init = {
      TokenType::T_STR, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::STRING_LITERAL, "abcd" } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{CHAR[] x = ['a', 'b', 'c', 'd']}}", std::move( res ) );
}

TEST_F( ParserTest, variable_declaration_with_array_of_basic_type ) {
  {
    std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                           TokenType::T_INT,
                                           TokenType::LBRACKET,
                                           TokenType::RBRACKET,
                                           { TokenType::IDENTIFIER, "x" },
                                           TokenType::OP_ASSIGN,
                                           TokenType::LBRACKET,
                                           { TokenType::INT_LITERAL, 1 },
                                           TokenType::RBRACKET };

    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var INT[] x = [1]}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::KW_VAR,
        TokenType::T_FLOAT,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        { TokenType::FLOAT_LITERAL, 1.f },
        TokenType::RBRACKET,
    };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var FLOAT[] x = [1.]}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::KW_VAR,
        TokenType::T_BOOL,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        { TokenType::BOOL_LITERAL, true },
        TokenType::RBRACKET,
    };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var BOOL[] x = [true]}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::KW_VAR,
        TokenType::T_CHAR,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        { TokenType::CHAR_LITERAL, 'c' },
        TokenType::RBRACKET,
    };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{var CHAR[] x = ['c']}}", std::move( res ) );
  }
}

TEST_F( ParserTest, constant_declaration_with_array_of_basic_type ) {
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_INT,     TokenType::LBRACKET, TokenType::RBRACKET,           { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN, TokenType::LBRACKET, { TokenType::INT_LITERAL, 1 }, TokenType::RBRACKET };

    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{INT[] x = [1]}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_FLOAT,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        { TokenType::FLOAT_LITERAL, 1.f },
        TokenType::RBRACKET,
    };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{FLOAT[] x = [1.]}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_BOOL,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        { TokenType::BOOL_LITERAL, true },
        TokenType::RBRACKET,
    };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{BOOL[] x = [true]}}", std::move( res ) );
  }
  {
    std::vector<TokenInitializer> init = {
        TokenType::T_CHAR,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        { TokenType::IDENTIFIER, "x" },
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        { TokenType::CHAR_LITERAL, 'c' },
        TokenType::RBRACKET,
    };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    EXPECT_EQ( "{{CHAR[] x = ['c']}}", std::move( res ) );
  }
}

TEST_F( ParserTest, variable_declaration_complex_array_type ) {
  std::vector<TokenInitializer> init = { TokenType::T_INT,
                                         TokenType::LBRACKET,
                                         TokenType::RBRACKET,
                                         TokenType::LBRACKET,
                                         TokenType::RBRACKET,
                                         TokenType::LBRACKET,
                                         TokenType::RBRACKET,

                                         { TokenType::IDENTIFIER, "mat3dim" },
                                         TokenType::OP_ASSIGN,

                                         TokenType::LBRACKET,
                                         TokenType::LBRACKET,
                                         TokenType::LBRACKET,
                                         { TokenType::INT_LITERAL, 1 },
                                         TokenType::RBRACKET,
                                         TokenType::RBRACKET,
                                         TokenType::RBRACKET,
                                         TokenType::NEWLINE,
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{INT[][][] mat3dim = [[[1]]]}}", std::move( res ) );
}

TEST_F( ParserTest, variable_declaration_string_array_type ) {
  std::vector<TokenInitializer> init = { TokenType::T_STR,
                                         TokenType::LBRACKET,
                                         TokenType::RBRACKET,

                                         { TokenType::IDENTIFIER, "alpha_and_bet" },
                                         TokenType::OP_ASSIGN,

                                         TokenType::LBRACKET,
                                         { TokenType::STRING_LITERAL, "abc" },
                                         TokenType::COMMA,
                                         { TokenType::STRING_LITERAL, "def" },
                                         TokenType::RBRACKET,
                                         TokenType::NEWLINE,
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{CHAR[][] alpha_and_bet = [['a', 'b', 'c'], ['d', 'e', 'f']]}}", std::move( res ) );
}

/* Advanced statement productions
 */

TEST_F( ParserTest, statement_function_def_with_array_type_and_modifiers ) {
  /*
  def void process(mut [int] arr, copy str name) do
  done
   */
  std::vector<TokenInitializer> init = { { TokenType::KW_DEF },
                                         { TokenType::T_VOID },
                                         { TokenType::IDENTIFIER, "process" },
                                         { TokenType::LPAREN },
                                         { TokenType::KW_MUT },
                                         { TokenType::T_INT },
                                         { TokenType::LBRACKET },
                                         { TokenType::RBRACKET },
                                         { TokenType::IDENTIFIER, "arr" },
                                         { TokenType::COMMA },
                                         { TokenType::KW_COPY },
                                         { TokenType::T_STR },
                                         { TokenType::IDENTIFIER, "name" },
                                         { TokenType::RPAREN },
                                         { TokenType::KW_DO },
                                         { TokenType::NEWLINE },
                                         { TokenType::KW_DONE },
                                         { TokenType::NEWLINE },
                                         { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{def VOID process(mut INT[] arr, copy CHAR[] name) {}}}", std::move( res ) );
}

TEST_F( ParserTest, statement_if_else ) {
  /*
  if (x > 0) do
    ret 1
  done else do
    ret 0
  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },          { TokenType::LPAREN },         { TokenType::IDENTIFIER, "x" },
      { TokenType::OP_GT },          { TokenType::INT_LITERAL, 0 }, { TokenType::RPAREN },
      { TokenType::KW_DO },          { TokenType::NEWLINE },        { TokenType::KW_RET },
      { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },        { TokenType::KW_DONE },
      { TokenType::KW_ELSE },        { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },         { TokenType::INT_LITERAL, 0 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },        { TokenType::NEWLINE },        { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{if ({x > 0}) {{ret 1}} else {{ret 0}}}}", std::move( res ) );
}

TEST_F( ParserTest, statement_if_elseif_elseif_elseif_else ) {
  /*
  if (true) do
    ret 1
  done elseif (true) do
    ret 2
  done elseif (true) do
    ret 3
  done elseif (true) do
    ret 4
  done else do
    ret 5
  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },     { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },    { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },    { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },

      { TokenType::KW_ELSEIF }, { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },    { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },    { TokenType::INT_LITERAL, 2 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },

      { TokenType::KW_ELSEIF }, { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },    { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },    { TokenType::INT_LITERAL, 3 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },

      { TokenType::KW_ELSEIF }, { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },    { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },    { TokenType::INT_LITERAL, 4 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },

      { TokenType::KW_ELSE },   { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },    { TokenType::INT_LITERAL, 5 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },

      { TokenType::NEWLINE },   { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ(
      "{{if (true) {{ret 1}} elseif (true) {{ret 2}} elseif (true) {{ret 3}} elseif (true) {{ret 4}} else {{ret 5}}}}",
      std::move( res ) );
}

/* Common mistakes
 */

TEST_F( ParserTest, variable_declaration_missing_assign_operator ) {
  std::vector<TokenInitializer> init = {
      TokenType::KW_VAR, TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, { TokenType::INT_LITERAL, 5 } };
  std::string res;
  ASSERT_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ), MissingOperatorException );
}

TEST_F( ParserTest, variable_declaration_missing_type ) {
  std::vector<TokenInitializer> init = {
      TokenType::KW_VAR, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 5 } };
  std::string res;
  ASSERT_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ), InvalidTypeException );
}