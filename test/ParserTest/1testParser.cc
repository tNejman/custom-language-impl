#include <gtest/gtest.h>

#include <cassert>

#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "TestHelperPars.hpp"

TEST_F( ParserTest, check_init2 ) {
  ASSERT_TRUE( true );
}

/* Program ::== StatementList EOF

 */

/* StatementList ::== { NEWLINE } { Statement NEWLINE { NEWLINE } }
  - 0,1,2+ leading newlines
  - 0,1,2+ statement-newline(0,1,2+ newline)

  - required newline after statement, possible many following
*/

void assertProgram( const ProgramNode* program_ptr, const unsigned int expected_statement_count ) {
  ASSERT_NE( nullptr, program_ptr );
  ASSERT_EQ( expected_statement_count, program_ptr->getStatementList().size() );
}

class ParserStatementListTest : public ParserTest {};

TEST_F( ParserStatementListTest, statement_list_possibilities ) {
  const int max_tested_leading_newlines = 3;
  const int max_tested_statement_followups = 3;
  const int max_tested_following_newlines = 3;

  for ( int i = 0; i < max_tested_leading_newlines; ++i ) {
    for ( int j = 0; j < max_tested_statement_followups; ++j ) {
      for ( int k = 0; k < max_tested_following_newlines; ++k ) {
        std::vector<TokenInitializer> init = {};
        for ( int leading_newline_count = 0; leading_newline_count < i; ++leading_newline_count ) {
          init.push_back( TokenType::NEWLINE );
        }
        for ( int statement_count = 0; statement_count < j; ++statement_count ) {
          init.push_back( TokenType::KW_RET );
          init.push_back( TokenType::NEWLINE );

          for ( int following_newlines = 0; following_newlines < k; ++following_newlines ) {
            init.push_back( TokenType::NEWLINE );
          }
        }
        auto program_ptr = initTokensAndBuildProgram( std::move( init ) );
        assertProgram( program_ptr.get(), j );
      }
    }
  }
}

TEST_F( ParserStatementListTest, statements_in_same_line ) {
  // ret ret
  ASSERT_THROW( auto program = initTokensAndBuildProgram( { TokenType::KW_RET, TokenType::KW_RET } ),
                MissingNewlineException );
}

TEST_F( ParserStatementListTest, no_newline_after_statement_before_eof ) {
  // ret EOF
  ASSERT_THROW( auto program = initTokensAndBuildProgram( { TokenType::KW_RET } ), MissingNewlineException );
}

/* Statement ::== FunctionDef
                | VarDecl
                | ConstDecl
                | IfStmt
                | WhileStmt
                | LoopControl
                | ReturnStmt
                | ExpressionStmt
 */

class ParserStatementTest : public ParserTest {};

TEST_F( ParserStatementTest, function_def ) {
  // def void foo() do
  // done
  auto program = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                              TokenType::T_VOID,
                                              { TokenType::IDENTIFIER, "foo" },
                                              TokenType::LPAREN,
                                              TokenType::RPAREN,
                                              TokenType::KW_DO,
                                              TokenType::NEWLINE,
                                              TokenType::KW_DONE,
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getFunctionList().size() );
  auto* function_def_ptr = dynamic_cast<const FunctionDefNode*>( program->getFunctionList()[0].get() );
  ASSERT_NE( nullptr, function_def_ptr );
}

TEST_F( ParserStatementTest, variable_declaration ) {
  // var int x = 1
  auto program = initTokensAndBuildProgram( { TokenType::KW_VAR,
                                              TokenType::T_INT,
                                              { TokenType::IDENTIFIER, "x" },
                                              TokenType::OP_ASSIGN,
                                              { TokenType::INT_LITERAL, 1 },
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* declaration_ptr = dynamic_cast<const VarOrConstDeclNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, declaration_ptr );
  ASSERT_EQ( Mutability::MUTABLE, declaration_ptr->getMutability() );
}

TEST_F( ParserStatementTest, constant_declaration ) {
  // int x = 1
  auto program = initTokensAndBuildProgram( { TokenType::T_INT,
                                              { TokenType::IDENTIFIER, "x" },
                                              TokenType::OP_ASSIGN,
                                              { TokenType::INT_LITERAL, 1 },
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* declaration_ptr = dynamic_cast<const VarOrConstDeclNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, declaration_ptr );
  ASSERT_EQ( Mutability::IMMUTABLE, declaration_ptr->getMutability() );
}

TEST_F( ParserStatementTest, if_stmt ) {
  // if (true) do
  // done
  auto program = initTokensAndBuildProgram( { TokenType::KW_IF,
                                              TokenType::LPAREN,
                                              { TokenType::BOOL_LITERAL, true },
                                              TokenType::RPAREN,
                                              TokenType::KW_DO,
                                              TokenType::NEWLINE,
                                              TokenType::KW_DONE,
                                              TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* if_stmt_ptr = dynamic_cast<const IfStatementNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, if_stmt_ptr );
}

TEST_F( ParserStatementTest, while_stmt ) {
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

TEST_F( ParserStatementTest, loop_control ) {
  // continue
  auto program = initTokensAndBuildProgram( { TokenType::KW_CONTINUE, TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* loop_control_ptr = dynamic_cast<const ControlFlowNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, loop_control_ptr );
}

TEST_F( ParserStatementTest, expression ) {
  // break
  auto program = initTokensAndBuildProgram( { { TokenType::BOOL_LITERAL, true }, TokenType::NEWLINE } );
  ASSERT_EQ( 1, program->getStatementList().size() );
  auto* expression_ptr = dynamic_cast<const IExpressionNode*>( program->getStatementList()[0].get() );
  ASSERT_NE( nullptr, expression_ptr );
}