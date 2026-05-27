#include <gtest/gtest.h>

#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "TestHelperPars.hpp"

class ParserWhileStmtTest : public ParserTest {};

/* WhileStmt ::== "while" ParenthExprAndBlock

*/

TEST_F( ParserWhileStmtTest, while_stmt ) {
  // while (true) do
  // done
  auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                              TokenType::LPAREN,
                                              { TokenType::BOOL_LITERAL, true },
                                              TokenType::RPAREN,
                                              TokenType::KW_DO,
                                              TokenType::NEWLINE,
                                              TokenType::KW_DONE,
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* while_stmt_ptr = dynamic_cast<const WhileStatementNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, while_stmt_ptr );
}

TEST_F( ParserWhileStmtTest, missing_opening_parenthesis ) {
  // while true) do
  // done
  try {
    auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                                { TokenType::BOOL_LITERAL, true },
                                                TokenType::RPAREN,
                                                TokenType::KW_DO,
                                                TokenType::NEWLINE,
                                                TokenType::KW_DONE,
                                                TokenType::NEWLINE } );
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::OPENING, e.getParenthesisType() );
  } catch ( ... ) {
    FAIL() << "caught something else";
  }
}

TEST_F( ParserWhileStmtTest, missing_expression ) {
  // while () do
  // done
  ASSERT_THROW( auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE, TokenType::LPAREN,

                                                            TokenType::RPAREN, TokenType::KW_DO, TokenType::NEWLINE,
                                                            TokenType::KW_DONE, TokenType::NEWLINE } ),
                MissingExpressionException );
}

TEST_F( ParserWhileStmtTest, missing_closing_parenthesis ) {
  // while (true do
  // done
  try {
    auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                                TokenType::LPAREN,
                                                { TokenType::BOOL_LITERAL, true },
                                                TokenType::KW_DO,
                                                TokenType::NEWLINE,
                                                TokenType::KW_DONE,
                                                TokenType::NEWLINE } );
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::CLOSING, e.getParenthesisType() );
  } catch ( ... ) {
    FAIL() << "caught something else";
  }
}

TEST_F( ParserWhileStmtTest, missing_kw_do ) {
  // while (true)
  // done
  try {
    auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                                TokenType::LPAREN,
                                                { TokenType::BOOL_LITERAL, true },
                                                TokenType::RPAREN,
                                                TokenType::NEWLINE,
                                                TokenType::KW_DONE,
                                                TokenType::NEWLINE } );
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DO, e.getTokenTypeKeyword() );
  } catch ( ... ) {
    FAIL() << "caught something else";
  }
}

TEST_F( ParserWhileStmtTest, missing_kw_done ) {
  // while (true) do
  try {
    auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                                TokenType::LPAREN,
                                                { TokenType::BOOL_LITERAL, true },
                                                TokenType::RPAREN,
                                                TokenType::KW_DO,
                                                TokenType::NEWLINE,
                                                TokenType::NEWLINE } );
  } catch ( const MissingKeywordException& e ) {
    ASSERT_EQ( TokenType::KW_DONE, e.getTokenTypeKeyword() );
  } catch ( ... ) {
    FAIL() << "caught something else";
  }
}

class ParserLoopControlTest : public ParserTest {};

TEST_F( ParserLoopControlTest, one_stmt ) {
  // while (true) do
  //     break
  // done
  auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                              TokenType::LPAREN,
                                              { TokenType::BOOL_LITERAL, true },
                                              TokenType::RPAREN,
                                              TokenType::KW_DO,
                                              TokenType::NEWLINE,
                                              TokenType::KW_BREAK,
                                              TokenType::NEWLINE,
                                              TokenType::KW_DONE,
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* while_stmt_ptr = dynamic_cast<const WhileStatementNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, while_stmt_ptr );
  ASSERT_EQ( 1, while_stmt_ptr->getBlock().size() );
  auto* control_loop_ptr = dynamic_cast<const ControlFlowNode*>( while_stmt_ptr->getBlock()[0].get() );
  ASSERT_NE( nullptr, control_loop_ptr );
  ASSERT_EQ( ControlFlowType::BREAK, control_loop_ptr->getControlFlowType() );
}

TEST_F( ParserLoopControlTest, two_stmt ) {
  // while (true) do
  //     break
  //     continue
  // done
  auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                              TokenType::LPAREN,
                                              { TokenType::BOOL_LITERAL, true },
                                              TokenType::RPAREN,
                                              TokenType::KW_DO,
                                              TokenType::NEWLINE,
                                              TokenType::KW_BREAK,
                                              TokenType::NEWLINE,
                                              TokenType::KW_CONTINUE,
                                              TokenType::NEWLINE,
                                              TokenType::KW_DONE,
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* while_stmt_ptr = dynamic_cast<const WhileStatementNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, while_stmt_ptr );
  ASSERT_EQ( 2, while_stmt_ptr->getBlock().size() );

  auto* control_loop_ptr1 = dynamic_cast<const ControlFlowNode*>( while_stmt_ptr->getBlock()[0].get() );
  ASSERT_NE( nullptr, control_loop_ptr1 );
  ASSERT_EQ( ControlFlowType::BREAK, control_loop_ptr1->getControlFlowType() );

  auto* control_loop_ptr2 = dynamic_cast<const ControlFlowNode*>( while_stmt_ptr->getBlock()[1].get() );
  ASSERT_NE( nullptr, control_loop_ptr2 );
  ASSERT_EQ( ControlFlowType::CONTINUE, control_loop_ptr2->getControlFlowType() );
}

TEST_F( ParserLoopControlTest, three_stmt ) {
  // while (true) do
  //     break
  //     continue
  //     break
  // done
  auto program = initTokensAndBuildProgram( { TokenType::KW_WHILE,
                                              TokenType::LPAREN,
                                              { TokenType::BOOL_LITERAL, true },
                                              TokenType::RPAREN,
                                              TokenType::KW_DO,
                                              TokenType::NEWLINE,
                                              TokenType::KW_BREAK,
                                              TokenType::NEWLINE,
                                              TokenType::KW_CONTINUE,
                                              TokenType::NEWLINE,
                                              TokenType::KW_BREAK,
                                              TokenType::NEWLINE,
                                              TokenType::KW_DONE,
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* while_stmt_ptr = dynamic_cast<const WhileStatementNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, while_stmt_ptr );
  ASSERT_EQ( 3, while_stmt_ptr->getBlock().size() );

  auto* control_loop_ptr1 = dynamic_cast<const ControlFlowNode*>( while_stmt_ptr->getBlock()[0].get() );
  ASSERT_NE( nullptr, control_loop_ptr1 );
  ASSERT_EQ( ControlFlowType::BREAK, control_loop_ptr1->getControlFlowType() );

  auto* control_loop_ptr2 = dynamic_cast<const ControlFlowNode*>( while_stmt_ptr->getBlock()[1].get() );
  ASSERT_NE( nullptr, control_loop_ptr2 );
  ASSERT_EQ( ControlFlowType::CONTINUE, control_loop_ptr2->getControlFlowType() );

  auto* control_loop_ptr3 = dynamic_cast<const ControlFlowNode*>( while_stmt_ptr->getBlock()[2].get() );
  ASSERT_NE( nullptr, control_loop_ptr3 );
  ASSERT_EQ( ControlFlowType::BREAK, control_loop_ptr3->getControlFlowType() );
}