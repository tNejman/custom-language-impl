#include "Exceptions/ParserExceptions/InvalidTypeException.hpp"
#include "Exceptions/ParserExceptions/MissingIdentifierException.hpp"
#include "Exceptions/ParserExceptions/MissingKeywordException.hpp"
#include "Exceptions/ParserExceptions/MissingNewlineException.hpp"
#include "Lexer/Token.hpp"
#include "TestHelperPars.hpp"

class ParserFunctionTest : public ParserTest {};

TEST_F( ParserFunctionTest, function_declaration_basic_type_no_arg_empty ) {
  /*
  def T foo() do
  done
  */
  std::vector<std::pair<TokenType, std::string>> basic_types = { { TokenType::T_BOOL, "BOOL" },
                                                                 { TokenType::T_CHAR, "CHAR" },
                                                                 { TokenType::T_FLOAT, "FLOAT" },
                                                                 { TokenType::T_INT, "INT" } };
  for ( const auto [tt, tt_str] : basic_types ) {
    std::vector<TokenInitializer> init = { TokenType::KW_DEF,
                                           tt,
                                           { TokenType::IDENTIFIER, "foo" },
                                           TokenType::LPAREN,
                                           TokenType::RPAREN,
                                           TokenType::KW_DO,
                                           TokenType::NEWLINE,
                                           TokenType::KW_DONE,
                                           TokenType::NEWLINE,
                                           TokenType::END_OF_FILE };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    std::string expected = "{{def ";
    expected += tt_str;
    expected += " foo() {}}}";
    EXPECT_EQ( std::move( expected ), std::move( res ) );
  }
}

TEST_F( ParserFunctionTest, function_declaration_array_basic_type_no_arg_empty ) {
  /*
  def T[] foo() do
  done
  */
  std::vector<std::pair<TokenType, std::string>> basic_types = { { TokenType::T_BOOL, "BOOL" },
                                                                 { TokenType::T_CHAR, "CHAR" },
                                                                 { TokenType::T_FLOAT, "FLOAT" },
                                                                 { TokenType::T_INT, "INT" } };
  for ( const auto [tt, tt_str] : basic_types ) {
    std::vector<TokenInitializer> init = { TokenType::KW_DEF,
                                           tt,
                                           TokenType::LBRACKET,
                                           TokenType::RBRACKET,
                                           { TokenType::IDENTIFIER, "foo" },
                                           TokenType::LPAREN,
                                           TokenType::RPAREN,
                                           TokenType::KW_DO,
                                           TokenType::NEWLINE,
                                           TokenType::KW_DONE,
                                           TokenType::NEWLINE,
                                           TokenType::END_OF_FILE };
    std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
    std::string expected = "{{def ";
    expected += tt_str;
    expected += "[] foo() {}}}";
    EXPECT_EQ( std::move( expected ), std::move( res ) );
  }
}

TEST_F( ParserFunctionTest, function_declaration_complex_return_type_no_arg_empty ) {
  /*
  def int[][][] foo() do
  done
  */
  std::vector<TokenInitializer> init = { TokenType::KW_DEF,     TokenType::T_INT,    TokenType::LBRACKET,
                                         TokenType::RBRACKET,   TokenType::LBRACKET, TokenType::RBRACKET,
                                         TokenType::LBRACKET,   TokenType::RBRACKET, { TokenType::IDENTIFIER, "foo" },
                                         TokenType::LPAREN,     TokenType::RPAREN,   TokenType::KW_DO,
                                         TokenType::NEWLINE,    TokenType::KW_DONE,  TokenType::NEWLINE,
                                         TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{def INT[][][] foo() {}}}", std::move( res ) );
}

TEST_F( ParserFunctionTest, statement_function_def_with_array_type_and_modifiers ) {
  /*
  def void process(mut int[] arr, copy str name) do
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

/* Function declaration mistakes
 */

TEST_F( ParserFunctionTest, function_declaration_missing_type ) {
  /*
  def foo() do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::RPAREN,
      TokenType::KW_DO,   TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), InvalidTypeException );
}

TEST_F( ParserFunctionTest, function_declaration_missing_identifier ) {
  /*
  def int () do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF, TokenType::T_INT,   TokenType::LPAREN,  TokenType::RPAREN,
      TokenType::KW_DO,  TokenType::NEWLINE, TokenType::KW_DONE, TokenType::NEWLINE,
  };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingIdentifierException );
}

TEST_F( ParserFunctionTest, function_declaration_missing_parameter_opening_parenthesis ) {
  /*
  def int foo) do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_INT,   { TokenType::IDENTIFIER, "foo" },
      TokenType::RPAREN,  TokenType::KW_DO,   TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::OPENING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserFunctionTest, function_declaration_missing_parameter_closing_parenthesis ) {
  /*
  def int foo( do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_INT,   { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::KW_DO,   TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::CLOSING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserFunctionTest, function_declaration_missing_kw_do ) {
  /*
  def int foo()
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_INT,   { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::RPAREN,  TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingKeywordException' was not thrown";
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DO, e.getTokenTypeKeyword() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingKeywordException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserFunctionTest, function_declaration_missing_kw_done ) {
  /*
  def int foo() do
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_INT,   { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::RPAREN,  TokenType::KW_DO,
      TokenType::NEWLINE, TokenType::NEWLINE,
  };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingKeywordException' was not thrown";
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DONE, e.getTokenTypeKeyword() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingKeywordException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserFunctionTest, function_declaration_missing_newline_after_do ) {
  /*
  def int foo() do done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_INT,   { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::RPAREN,  TokenType::KW_DO,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingNewlineException );
}

TEST_F( ParserFunctionTest, function_declaration_missing_newline_after_done ) {
  /*
  def int foo() do done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_INT,   { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::RPAREN,  TokenType::KW_DO,
      TokenType::NEWLINE, TokenType::KW_DONE,
  };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingNewlineException );
}