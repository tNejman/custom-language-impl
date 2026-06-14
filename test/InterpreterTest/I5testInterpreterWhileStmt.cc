#include <gtest/gtest.h>

#include <cassert>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterWhileStmtTest : public InterpreterTest {};

TEST_F( InterpreterWhileStmtTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterWhileStmtTest, evaluate_true_and_enter_body ) {
  /*
  while (true) do
    break
  done
  */
  MAKE_STATEMENTS( MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_BREAK() ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<ControlFlowNode>( IT, 1 );
}

TEST_F( InterpreterWhileStmtTest, evaluate_false_and_skip_body ) {
  /*
  while (false) do
    int x = 1
  done
  */
  MAKE_STATEMENTS( MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, false ), makeBlock( MAKE_CONST_INT( "x", 1 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<VarOrConstDeclNode>( IT, 0 );  // count bodies
}

TEST_F( InterpreterWhileStmtTest, cond_not_bool ) {
  /*
  while (1) do
  done
  */
  MAKE_STATEMENTS( MAKE_WHILE( MAKE_LITERAL( BaseType::INT, 1 ), makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), NotAllowedTypeException );
}

TEST_F( InterpreterWhileStmtTest, void_cond ) {
  /*
  var int x = 1
  while (x = 2) do
  done
  */
  MAKE_STATEMENTS(
      MAKE_VAR_INT( "x", 1 ),
      MAKE_WHILE( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ), makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), VoidValueException );
}

TEST_F( InterpreterWhileStmtTest, dynamic_cond_eval ) {
  /*
  var bool x = true
  while (x) do
    x = false
  done
  */
  MAKE_STATEMENTS( MAKE_DECL_VAR( "x", BaseType::BOOL, MAKE_LITERAL( BaseType::BOOL, true ) ),
                   MAKE_WHILE( MAKE_ID( "x" ), makeBlock( MAKE_ASSIGNMENT_EXPR(
                                                   MAKE_ID( "x" ), MAKE_LITERAL( BaseType::BOOL, false ) ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<WhileStatementNode>( IT, 1 );
}

TEST_F( InterpreterWhileStmtTest, dynamic_cond_eval_multiple_times ) {
  /*
  var int x = 0
  while (x < 3) do
    x = x + 1
  done
  */
  MAKE_STATEMENTS( MAKE_VAR_INT( "x", 0 ),
                   MAKE_WHILE( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 3 ), BinaryOperator::LT ),
                               makeBlock( MAKE_ASSIGNMENT_EXPR(
                                   MAKE_ID( "x" ), MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 1 ),
                                                                     BinaryOperator::ADD ) ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<AssignmentExprNode>( IT, 3 );
}

TEST_F( InterpreterWhileStmtTest, decl_inside_body_no_leak ) {
  /*
  while (true) do
      var int x = 1
      break
  done
  x = 2
  */
  MAKE_STATEMENTS(
      MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_VAR_INT( "x", 1 ), MAKE_BREAK() ) ),
      MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), UnknownIdentifierException );
}

TEST_F( InterpreterWhileStmtTest, can_mutate_outer_variables ) {
  /*
  var int x = 1
  while (true) do
      x = 2
      break
  done
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto while_stmt =
      MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ),
                  makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ), MAKE_BREAK() ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  IT.visit( *while_stmt );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), 2 );
}

TEST_F( InterpreterWhileStmtTest, scope_decl_not_pollute_global ) {
  /*
  while (true) do
      int x = 1
      break
  done
  */
  MAKE_STATEMENTS(
      MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 1 ), MAKE_BREAK() ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const WhileStatementNode*>( &node ) ) {
      // same state expected before and after
      assertCallStackSize( IT, 1u );
      assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
      assertTopCallContextVarCount( IT, 0u );
    }
  } );
  IT.execute();
}

TEST_F( InterpreterWhileStmtTest, void_left_in_acc ) {
  /*
  while (true) do
    break
  done
  */
  MAKE_STATEMENTS( MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_BREAK() ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const WhileStatementNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertAccSize( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertAccSize( IT, 1u );
          ASSERT_TRUE( ITF::acc( IT ).top().isVoid() && "statement call ought to leave void in acc" );
          break;
        }
      }
    }
  } );
  IT.execute();
}

/* CONTROL FLOW



*/

TEST_F( InterpreterWhileStmtTest, break_breaks ) {
  /*
  while (true) do
    break

  done
  */
  MAKE_STATEMENTS( MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_BREAK() ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<WhileStatementNode, ControlFlowNode>( IT, 1, 1 );
}

TEST_F( InterpreterWhileStmtTest, break_skips_instructions_after ) {
  /*
  while (true) do
    break
    int x = 1
  done
  */
  MAKE_STATEMENTS(
      MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_BREAK(), MAKE_CONST_INT( "x", 1 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<WhileStatementNode, ControlFlowNode, VarOrConstDeclNode>( IT, 1, 1, 0 );
}

TEST_F( InterpreterWhileStmtTest, continue_skips_current_loop ) {
  /*
  var bool x = false
  var int y = 0
  while (y < 3) do
    y = y + 1
    continue
    x = true
  done
  /// x must be false
  */
  MAKE_STATEMENTS(
      MAKE_DECL_VAR( "x", BaseType::BOOL, MAKE_LITERAL( BaseType::BOOL, false ) ), MAKE_VAR_INT( "y", 0 ),
      MAKE_WHILE(
          MAKE_BINARY_EXPR( MAKE_ID( "y" ), MAKE_LITERAL( BaseType::INT, 3 ), BinaryOperator::LT ),
          makeBlock(
              MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_BINARY_EXPR( MAKE_ID( "y" ), MAKE_LITERAL( BaseType::INT, 1 ),
                                                                      BinaryOperator::ADD ) ),
              MAKE_CONTINUE(), MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::BOOL, true ) ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  int while_body_counter = 0;
  int continue_counter = 0;
  int assignment_counter = 0;
  IT.setDebugHook( [&]( Interpreter& interpreter, const INode& node, DebugEvent event ) mutable {
    if ( event != DebugEvent::AFTER_NODE_VISIT ) {
      return;
    }
    if ( dynamic_cast<const WhileStatementNode*>( &node ) ) {
      ++while_body_counter;
      if ( while_body_counter >= 1 ) {
        ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), false );
      }
    } else if ( dynamic_cast<const ControlFlowNode*>( &node ) ) {
      ++continue_counter;
    } else if ( dynamic_cast<const AssignmentExprNode*>( &node ) ) {
      ++assignment_counter;
    }
  } );
  IT.execute();

  ASSERT_EQ( 1, while_body_counter );
  ASSERT_EQ( 3, continue_counter );
  ASSERT_EQ( 3, assignment_counter );
}

TEST_F( InterpreterWhileStmtTest, break_not_in_while ) {
  /*
  break
  */
  MAKE_STATEMENTS( MAKE_BREAK() );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), InvalidStatementException );
}

TEST_F( InterpreterWhileStmtTest, continue_not_in_while ) {
  /*
  continue
  */
  MAKE_STATEMENTS( MAKE_CONTINUE() );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), InvalidStatementException );
}

TEST_F( InterpreterWhileStmtTest, break_through_scope ) {
  /*
  while (true) do
    if (true) do
      break
    done
    int x = 1
  done
  */
  MAKE_STATEMENTS( MAKE_WHILE(
      MAKE_LITERAL( BaseType::BOOL, true ),
      makeBlock( MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_BREAK() ) ),
                          makeBlock() ),
                 MAKE_CONST_INT( "x", 1 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<WhileStatementNode, IfStatementNode, ControlFlowNode, VarOrConstDeclNode>( IT, 1, 1, 1,
                                                                                                        0 );
}

TEST_F( InterpreterWhileStmtTest, not_break_through_function ) {
  /*
  def void foo() do
    break
  done

  while (true) do
    foo()
  done
  */
  MAKE_STATEMENTS(
      MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ),
                  makeBlock( std::make_unique<FunctionCallNode>( Position{ 1, 1 }, "foo", makeExpressions() ) ) ) );
  MAKE_FUNCTIONS( std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(),
                                                     makeBlock( MAKE_BREAK() ) ) );
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), InvalidStatementException );
}

TEST_F( InterpreterWhileStmtTest, break_only_one_loop ) {
  /*
  while (true) do
    while (true) do
      break
    done
    int x = 1
    break
  done
  */
  MAKE_STATEMENTS( MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ),
                               makeBlock( MAKE_WHILE( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_BREAK() ) ),
                                          MAKE_CONST_INT( "x", 1 ), MAKE_BREAK() ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<WhileStatementNode, ControlFlowNode, VarOrConstDeclNode>( IT, 2, 2, 1 );
}