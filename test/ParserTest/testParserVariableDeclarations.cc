#include "Lexer/Token.hpp"
#include "TestHelperPars.hpp"

class ParserVariableDeclarationTest : public ParserTest {};

TEST_F( ParserVariableDeclarationTest, variable_declaration ) {
  std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                         TokenType::T_INT,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::INT_LITERAL, 0 },
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var INT x = 0}}", std::move( res ) );
}

TEST_F( ParserVariableDeclarationTest, constant_declaration ) {
  std::vector<TokenInitializer> init = { TokenType::T_INT,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::INT_LITERAL, 0 },
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{INT x = 0}}", std::move( res ) );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_basic_type ) {
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

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_basic_type ) {
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

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_string_type ) {
  std::vector<TokenInitializer> init = { TokenType::KW_VAR,
                                         TokenType::T_STR,
                                         { TokenType::IDENTIFIER, "x" },
                                         TokenType::OP_ASSIGN,
                                         { TokenType::STRING_LITERAL, "abcd" } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{var CHAR[] x = ['a', 'b', 'c', 'd']}}", std::move( res ) );
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_string_type ) {
  std::vector<TokenInitializer> init = {
      TokenType::T_STR, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::STRING_LITERAL, "abcd" } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{CHAR[] x = ['a', 'b', 'c', 'd']}}", std::move( res ) );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_array_of_basic_type ) {
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

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_array_of_basic_type ) {
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

TEST_F( ParserVariableDeclarationTest, variable_declaration_complex_array_type ) {
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

TEST_F( ParserVariableDeclarationTest, variable_declaration_string_array_type ) {
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

/* Errors in variable declaration
 */

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_type ) {
  std::vector<TokenInitializer> init = {
      TokenType::KW_VAR, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 5 } };
  std::string res;
  ASSERT_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ), InvalidTypeException );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_identifier ) {
  std::vector<TokenInitializer> init = {
      TokenType::KW_VAR, TokenType::T_INT, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 5 } };
  std::string res;
  ASSERT_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ), MissingIdentifierException );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_assign_operator ) {
  std::vector<TokenInitializer> init = {
      TokenType::KW_VAR, TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, { TokenType::INT_LITERAL, 5 } };
  std::string res;
  ASSERT_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ), MissingOperatorException );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_initializer_expression ) {
  std::vector<TokenInitializer> init = {
      TokenType::KW_VAR, TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN };
  std::string res;
  ASSERT_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ), MissingExpressionException );
}