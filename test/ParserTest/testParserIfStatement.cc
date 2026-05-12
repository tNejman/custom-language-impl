#include "TestHelperPars.hpp"

class ParserIfStatementTest : public ParserTest {};

TEST_F( ParserIfStatementTest, statement_if ) {
  std::vector<TokenInitializer> init = { TokenType::KW_IF,   TokenType::LPAREN,  { TokenType::BOOL_LITERAL, true },
                                         TokenType::RPAREN,  TokenType::KW_DO,   TokenType::NEWLINE,
                                         TokenType::KW_DONE, TokenType::NEWLINE, TokenType::END_OF_FILE };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{if (true) {} else {}}}", std::move( res ) );
}

TEST_F( ParserIfStatementTest, statement_if_else ) {
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

TEST_F( ParserIfStatementTest, statement_if_elseif_elseif_elseif_else ) {
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

TEST_F( ParserIfStatementTest, statement_if_else_nested_many_times ) {
  /*
  if (true) do
    if (true) do
      if (true) do
        ret 11
      done else do
        ret 12
      done
    done else do
      ret 21
    done
  done else do
    ret 31
  done
  */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },      { TokenType::LPAREN },          { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },     { TokenType::KW_DO },           { TokenType::NEWLINE },

      { TokenType::KW_IF },      { TokenType::LPAREN },          { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },     { TokenType::KW_DO },           { TokenType::NEWLINE },

      { TokenType::KW_IF },      { TokenType::LPAREN },          { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },     { TokenType::KW_DO },           { TokenType::NEWLINE },

      { TokenType::KW_RET },     { TokenType::INT_LITERAL, 11 }, { TokenType::NEWLINE },

      { TokenType::KW_DONE },    { TokenType::KW_ELSE },         { TokenType::KW_DO },
      { TokenType::NEWLINE },    { TokenType::KW_RET },          { TokenType::INT_LITERAL, 12 },
      { TokenType::NEWLINE },    { TokenType::KW_DONE },         { TokenType::NEWLINE },

      { TokenType::KW_DONE },    { TokenType::KW_ELSE },         { TokenType::KW_DO },
      { TokenType::NEWLINE },    { TokenType::KW_RET },          { TokenType::INT_LITERAL, 21 },
      { TokenType::NEWLINE },    { TokenType::KW_DONE },         { TokenType::NEWLINE },

      { TokenType::KW_DONE },    { TokenType::KW_ELSE },         { TokenType::KW_DO },
      { TokenType::NEWLINE },    { TokenType::KW_RET },          { TokenType::INT_LITERAL, 31 },
      { TokenType::NEWLINE },    { TokenType::KW_DONE },         { TokenType::NEWLINE },
      { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{if (true) {{if (true) {{if (true) {{ret 11}} else {{ret 12}}}} else {{ret 21}}}} else {{ret 31}}}}",
             std::move( res ) );
}

TEST_F( ParserIfStatementTest, statement_if_else_nested_ambiguous_binding ) {
  /*
  if (true) do
    if (true) do
      ret 1
    done
  done else do
    ret 2
  done */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },      { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },     { TokenType::KW_DO },          { TokenType::NEWLINE },

      { TokenType::KW_IF },      { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },     { TokenType::KW_DO },          { TokenType::NEWLINE },

      { TokenType::KW_RET },     { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE },    { TokenType::NEWLINE },

      { TokenType::KW_DONE },    { TokenType::KW_ELSE },        { TokenType::KW_DO },
      { TokenType::NEWLINE },    { TokenType::KW_RET },         { TokenType::INT_LITERAL, 2 },
      { TokenType::NEWLINE },    { TokenType::KW_DONE },        { TokenType::NEWLINE },
      { TokenType::END_OF_FILE } };
  std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
  EXPECT_EQ( "{{if (true) {{if (true) {{ret 1}} else {}}} else {{ret 2}}}}", std::move( res ) );
}

/*
 ===
  Errors in if statements
 ===
 */

TEST_F( ParserIfStatementTest, statement_if_missing_opening_condition_parenthesis ) {
  /*
  if true) do
    ret 1
  done

   */
  std::vector<TokenInitializer> init = { { TokenType::KW_IF },          { TokenType::BOOL_LITERAL, true },
                                         { TokenType::RPAREN },         { TokenType::KW_DO },
                                         { TokenType::NEWLINE },        { TokenType::KW_RET },
                                         { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
                                         { TokenType::KW_DONE },        { TokenType::NEWLINE },
                                         { TokenType::END_OF_FILE } };

  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::OPENING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, statement_if_missing_condition ) {
  /*
  if () do
    ret 1
  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN },  { TokenType::RPAREN },         { TokenType::KW_DO },
      { TokenType::NEWLINE }, { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE }, { TokenType::NEWLINE }, { TokenType::END_OF_FILE } };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingExpressionException );
}

TEST_F( ParserIfStatementTest, statement_if_missing_closing_condition_parenthesis ) {
  /*
  if (true do
    ret 1
  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN },  { TokenType::BOOL_LITERAL, true }, { TokenType::KW_DO },
      { TokenType::NEWLINE }, { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 },     { TokenType::NEWLINE },
      { TokenType::KW_DONE }, { TokenType::NEWLINE }, { TokenType::END_OF_FILE } };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::CLOSING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, statement_if_missing_kw_do ) {
  /*
  if (true)
    ret 1
  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN },  { TokenType::BOOL_LITERAL, true }, { TokenType::RPAREN },
      { TokenType::NEWLINE }, { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 },     { TokenType::NEWLINE },
      { TokenType::KW_DONE }, { TokenType::NEWLINE }, { TokenType::END_OF_FILE } };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingKeywordException' was not thrown";
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DO, e.getTokenTypeKeyword() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingKeywordException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, statement_if_missing_statements_in_block ) {
  /*
  if (true) do

  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN },  { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },  { TokenType::KW_DO },   { TokenType::NEWLINE },
      { TokenType::KW_DONE }, { TokenType::NEWLINE }, { TokenType::END_OF_FILE } };

  std::string res;
  ASSERT_NO_THROW( res = initTokensBuildProgramAndSerialize( std::move( init ) ) );
  ASSERT_EQ( "{{if (true) {} else {}}}", res );
}

TEST_F( ParserIfStatementTest, statement_if_missing_kw_done ) {
  /*
  if (true) do
    ret 1

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
      { TokenType::NEWLINE }, { TokenType::END_OF_FILE } };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingKeywordException' was not thrown";
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DONE, e.getTokenTypeKeyword() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingKeywordException' but a different exception was thrown, with msg: " << re.what();
  }
}