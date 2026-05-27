#include <cassert>

#include "Exceptions/ParserExceptions/MissingNewlineException.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "TestHelperPars.hpp"

void assertIfStatement( const IfStatementNode* node, int elseif_count ) {
  ASSERT_NE( nullptr, node );
  ASSERT_TRUE( node->getCondBlockPairs().size() >= 1 );
  for ( const auto& [cond, _] : node->getCondBlockPairs() ) {
    ASSERT_NE( nullptr, cond );
  }
  ASSERT_EQ( elseif_count, node->getCondBlockPairs().size() - 1 );
}

void assertStatementIsRetLiteral( const INode* node, int val ) {
  const auto* ret_ptr = dynamic_cast<const ReturnNode*>( node );
  ASSERT_NE( nullptr, ret_ptr );
  const auto* literal_ptr = dynamic_cast<const LiteralExprNode*>( ret_ptr->getExpression() );
  ASSERT_NE( nullptr, literal_ptr );
  ASSERT_EQ( literal_ptr->getType(), BaseType::INT );
  ASSERT_EQ( val, std::get<int>( literal_ptr->getValue().getData() ) );
}

/* IfStmt ::== "if" ParenthExprAndBlock
               { "elseif" ParenthExprAndBlock }
               [ "else" Block ]
               NEWLINE

  if_stmt = if parenth_expr_and_block 0*(elseif parenth_expr_and_block)
  if_stmt = if parenth_expr_and_block 1*(elseif parenth_expr_and_block)
  if_stmt = if parenth_expr_and_block 2+*(elseif parenth_expr_and_block)

  if_stmt = if parenth_expr_and_block 0*(elseif parenth_expr_and_block) else block
  if_stmt = if parenth_expr_and_block 1*(elseif parenth_expr_and_block) else block
  if_stmt = if parenth_expr_and_block 2+*(elseif parenth_expr_and_block) else block
*/
class ParserIfStatementTest : public ParserTest {};

TEST_F( ParserIfStatementTest, if_stmt ) {
  /*
  if (true) do
  done
  */
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_IF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* if_ptr = dynamic_cast<const IfStatementNode*>( program_ptr->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 0 );
}

TEST_F( ParserIfStatementTest, if_stmt_one_elseif ) {
  /*
  if (true) do
  done elseif (true) do
  done
  */
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_IF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSEIF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* if_ptr = dynamic_cast<const IfStatementNode*>( program_ptr->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 1 );
}

TEST_F( ParserIfStatementTest, if_stmt_more_than_one_elseif ) {
  /*
  if (true) do
  done elseif (true) do
  done elseif (true) do
  done
  */
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_IF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSEIF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSEIF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* if_ptr = dynamic_cast<const IfStatementNode*>( program_ptr->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 2 );
}

TEST_F( ParserIfStatementTest, if_stmt_else ) {
  /*
  if (true) do
  done else do
  done
  */
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_IF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSE,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* if_ptr = dynamic_cast<const IfStatementNode*>( program_ptr->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 0 );
}

TEST_F( ParserIfStatementTest, if_stmt_one_elseif_else ) {
  /*
  if (true) do
  done elseif (true) do
  done else do
  done
  */
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_IF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSEIF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSE,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* if_ptr = dynamic_cast<const IfStatementNode*>( program_ptr->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 1 );
}

TEST_F( ParserIfStatementTest, if_stmt_more_than_one_elseif_else ) {
  /*
  if (true) do
  done elseif (true) do
  done elseif (true) do
  done else do
  done
  */
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_IF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSEIF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSEIF,
                                                  TokenType::LPAREN,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,

                                                  TokenType::KW_DONE,
                                                  TokenType::KW_ELSE,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* if_ptr = dynamic_cast<const IfStatementNode*>( program_ptr->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 2 );
}

/* other than base production rules
 */

TEST_F( ParserIfStatementTest, nested ) {
  /*
  if (true) do
      if (true) do
          ret 1
      done else do
          ret 2
      done
  done else do
      ret 3
  done
  */
  auto program = initTokensAndBuildProgram(
      { { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },

        { TokenType::KW_DONE }, { TokenType::KW_ELSE },        { TokenType::KW_DO },
        { TokenType::NEWLINE }, { TokenType::KW_RET },         { TokenType::INT_LITERAL, 2 },
        { TokenType::NEWLINE }, { TokenType::KW_DONE },        { TokenType::NEWLINE },

        { TokenType::KW_DONE }, { TokenType::KW_ELSE },        { TokenType::KW_DO },
        { TokenType::NEWLINE }, { TokenType::KW_RET },         { TokenType::INT_LITERAL, 3 },
        { TokenType::NEWLINE }, { TokenType::KW_DONE },        TokenType::NEWLINE } );

  ASSERT_EQ( 1, program->getStatementList().size() );
  const auto* if_ptr = dynamic_cast<const IfStatementNode*>( program->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 0 );
  ASSERT_EQ( 1, if_ptr->getElseBlock().size() );
  assertStatementIsRetLiteral( if_ptr->getElseBlock()[0].get(), 3 );

  const auto& if_body = if_ptr->getCondBlockPairs()[0].second;
  ASSERT_EQ( 1, if_body.size() );
  const auto* if_nested_ptr = dynamic_cast<const IfStatementNode*>( if_body[0].get() );
  assertIfStatement( if_nested_ptr, 0 );
  assertStatementIsRetLiteral( if_nested_ptr->getElseBlock()[0].get(), 2 );

  assertStatementIsRetLiteral( if_nested_ptr->getCondBlockPairs()[0].second[0].get(), 1 );
}

TEST_F( ParserIfStatementTest, nested_twice ) {
  /*
  if (true) do
      if (true) do
          if (true) do
              ret 1
          done else do
              ret 2
          done
      done else do
          ret 3
      done
  done else do
      ret 4
  done
  */
  auto program = initTokensAndBuildProgram(
      { { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },

        { TokenType::KW_DONE }, { TokenType::KW_ELSE },        { TokenType::KW_DO },
        { TokenType::NEWLINE }, { TokenType::KW_RET },         { TokenType::INT_LITERAL, 2 },
        { TokenType::NEWLINE }, { TokenType::KW_DONE },        { TokenType::NEWLINE },

        { TokenType::KW_DONE }, { TokenType::KW_ELSE },        { TokenType::KW_DO },
        { TokenType::NEWLINE }, { TokenType::KW_RET },         { TokenType::INT_LITERAL, 3 },
        { TokenType::NEWLINE }, { TokenType::KW_DONE },        { TokenType::NEWLINE },

        { TokenType::KW_DONE }, { TokenType::KW_ELSE },        { TokenType::KW_DO },
        { TokenType::NEWLINE }, { TokenType::KW_RET },         { TokenType::INT_LITERAL, 4 },
        { TokenType::NEWLINE }, { TokenType::KW_DONE },        { TokenType::NEWLINE } } );

  ASSERT_EQ( 1, program->getStatementList().size() );
  const auto* if_ptr = dynamic_cast<const IfStatementNode*>( program->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 0 );
  ASSERT_EQ( 1, if_ptr->getElseBlock().size() );
  assertStatementIsRetLiteral( if_ptr->getElseBlock()[0].get(), 4 );

  const auto& if_body = if_ptr->getCondBlockPairs()[0].second;
  ASSERT_EQ( 1, if_body.size() );
  const auto* if_nested_ptr = dynamic_cast<const IfStatementNode*>( if_body[0].get() );
  assertIfStatement( if_nested_ptr, 0 );
  assertStatementIsRetLiteral( if_nested_ptr->getElseBlock()[0].get(), 3 );

  const auto& if_nested_body = if_nested_ptr->getCondBlockPairs()[0].second;
  ASSERT_EQ( 1, if_nested_body.size() );
  const auto* if_nested_nested_ptr = dynamic_cast<const IfStatementNode*>( if_nested_body[0].get() );
  assertIfStatement( if_nested_nested_ptr, 0 );
  assertStatementIsRetLiteral( if_nested_nested_ptr->getElseBlock()[0].get(), 2 );

  assertStatementIsRetLiteral( if_nested_nested_ptr->getCondBlockPairs()[0].second[0].get(), 1 );
}

TEST_F( ParserIfStatementTest, nested_ambiguous_binding ) {
  /*
  if (true) do
    if (true) do
      ret 1
    done
  done else do
    ret 2
  done */
  auto program = initTokensAndBuildProgram(
      { { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_IF },   { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
        { TokenType::RPAREN },  { TokenType::KW_DO },          { TokenType::NEWLINE },

        { TokenType::KW_RET },  { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
        { TokenType::KW_DONE }, { TokenType::NEWLINE },

        { TokenType::KW_DONE }, { TokenType::KW_ELSE },        { TokenType::KW_DO },
        { TokenType::NEWLINE }, { TokenType::KW_RET },         { TokenType::INT_LITERAL, 2 },
        { TokenType::NEWLINE }, { TokenType::KW_DONE },        TokenType::NEWLINE } );

  ASSERT_EQ( 1, program->getStatementList().size() );
  const auto* if_ptr = dynamic_cast<const IfStatementNode*>( program->getStatementList()[0].get() );
  assertIfStatement( if_ptr, 0 );
  ASSERT_EQ( 1, if_ptr->getElseBlock().size() );
  assertStatementIsRetLiteral( if_ptr->getElseBlock()[0].get(), 2 );

  const auto& if_body = if_ptr->getCondBlockPairs()[0].second;
  ASSERT_EQ( 1, if_body.size() );
  const auto* if_nested_ptr = dynamic_cast<const IfStatementNode*>( if_body[0].get() );
  assertIfStatement( if_nested_ptr, 0 );

  assertStatementIsRetLiteral( if_nested_ptr->getCondBlockPairs()[0].second[0].get(), 1 );
}

/*
 ===
  Errors in if statements
 ===
 */

TEST_F( ParserIfStatementTest, missing_opening_condition_parenthesis ) {
  /*
  if true) do
    ret 1
  done

   */
  std::vector<TokenInitializer> init = { { TokenType::KW_IF },          { TokenType::BOOL_LITERAL, true },
                                         { TokenType::RPAREN },         { TokenType::KW_DO },
                                         { TokenType::NEWLINE },        { TokenType::KW_RET },
                                         { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
                                         { TokenType::KW_DONE },        TokenType::NEWLINE };

  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::OPENING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, missing_condition ) {
  /*
  if () do
    ret 1
  done
   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN }, { TokenType::RPAREN },         { TokenType::KW_DO },
      { TokenType::NEWLINE }, { TokenType::KW_RET }, { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE },
      { TokenType::KW_DONE }, TokenType::NEWLINE };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingExpressionException );
}

TEST_F( ParserIfStatementTest, missing_closing_condition_parenthesis ) {
  /*
  if (true do
    ret 1
  done
   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN }, { TokenType::BOOL_LITERAL, true }, { TokenType::KW_DO },
      { TokenType::NEWLINE }, { TokenType::KW_RET }, { TokenType::INT_LITERAL, 1 },     { TokenType::NEWLINE },
      { TokenType::KW_DONE }, TokenType::NEWLINE };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::CLOSING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, missing_kw_do ) {
  /*
  if (true)
    ret 1
  done
   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN }, { TokenType::BOOL_LITERAL, true }, { TokenType::RPAREN },
      { TokenType::NEWLINE }, { TokenType::KW_RET }, { TokenType::INT_LITERAL, 1 },     { TokenType::NEWLINE },
      { TokenType::KW_DONE }, TokenType::NEWLINE };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingKeywordException' was not thrown";
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DO, e.getTokenTypeKeyword() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingKeywordException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, missing_newline_after_kw_do ) {
  /*
  if (true) do ret 1
  done
   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN }, { TokenType::BOOL_LITERAL, true }, { TokenType::RPAREN },
      { TokenType::KW_DO },   { TokenType::KW_RET }, { TokenType::INT_LITERAL, 1 },     { TokenType::NEWLINE },
      { TokenType::KW_DONE }, TokenType::NEWLINE };

  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingNewlineException );
}

TEST_F( ParserIfStatementTest, statement_if_missing_statements_in_block ) {
  /*
  if (true) do

  done

   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },   { TokenType::LPAREN }, { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN },  { TokenType::KW_DO },  { TokenType::NEWLINE },
      { TokenType::KW_DONE }, TokenType::NEWLINE };

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
      { TokenType::KW_IF },  { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN }, { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET }, { TokenType::INT_LITERAL, 1 }, { TokenType::NEWLINE } };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingKeywordException' was not thrown";
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DONE, e.getTokenTypeKeyword() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingKeywordException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserIfStatementTest, missing_newline_before_kw_done ) {
  /*
  if (true) do
    ret 1 done
   */
  std::vector<TokenInitializer> init = {
      { TokenType::KW_IF },  { TokenType::LPAREN },         { TokenType::BOOL_LITERAL, true },
      { TokenType::RPAREN }, { TokenType::KW_DO },          { TokenType::NEWLINE },
      { TokenType::KW_RET }, { TokenType::INT_LITERAL, 1 }, { TokenType::KW_DONE },
      TokenType::NEWLINE };

  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingNewlineException );
}