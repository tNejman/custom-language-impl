#include <gtest/gtest.h>

#include <cassert>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Exceptions/ParserExceptions/InvalidTypeException.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterIfStmtTest : public InterpreterTest {};

TEST_F( InterpreterIfStmtTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterIfStmtTest, evaluate_true_and_enter_body ) {
  /*
  if (true) do
      int x = 1
  done else do
      int y = 2
  done
  */
  MAKE_STATEMENTS(
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 1 ) ) ),
               makeBlock( MAKE_CONST_INT( "y", 2 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 2u );
          assertTopCallContextType( IT, CallContext::ContextType::IF_BLOCK );
          assertTopCallContextVarCount( IT, 0u );
          assertAccSize( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 2u );
          assertTopCallContextType( IT, CallContext::ContextType::IF_BLOCK );
          assertTopCallContextVarCount( IT, 1u );
          Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterIfStmtTest, evaluate_false_and_enter_else ) {
  /*
  if (false) do
      int x = 1
  done else do
      int y = 2
  done
  */
  MAKE_STATEMENTS(
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, false ), makeBlock( MAKE_CONST_INT( "x", 1 ) ) ),
               makeBlock( MAKE_CONST_INT( "y", 2 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 2u );
          assertTopCallContextType( IT, CallContext::ContextType::IF_BLOCK );
          assertTopCallContextVarCount( IT, 0u );
          assertAccSize( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 2u );
          assertTopCallContextType( IT, CallContext::ContextType::IF_BLOCK );
          assertTopCallContextVarCount( IT, 1u );
          Variable var_comp{ "y", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 2 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterIfStmtTest, if_else_enter_only_1_branch ) {
  /*
  if (true) do
      int x = 1
  done else do
      int x = 2
  done
  */
  MAKE_STATEMENTS(
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 1 ) ) ),
               makeBlock( MAKE_CONST_INT( "x", 2 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<IfStatementNode>( IT, 1 );
}

TEST_F( InterpreterIfStmtTest, if_else_if_multiple_true_enter_only_1_branch ) {
  /*
  if (true) do
      int x = 1
  done elseif (true) do
      int x = 2
  done else if (true) do
      int x = 3
  done else do
      int x = 4
  done
  */
  MAKE_STATEMENTS(
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 1 ) ),
                                     MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 2 ) ),
                                     MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 3 ) ) ),
               makeBlock( MAKE_CONST_INT( "x", 4 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  executeAndAssertNodeVisits<IfStatementNode>( IT, 1 );
}

TEST_F( InterpreterIfStmtTest, expr_type_not_bool ) {
  /*
  if (1) do
  done
  */
  MAKE_STATEMENTS( MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::INT, 1 ), makeBlock() ), makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER
  ASSERT_THROW( IT.execute(), InvalidTypeException );
}

TEST_F( InterpreterIfStmtTest, decl_inside_body_no_leak ) {
  /*
  if (true) do
      int x = 1
  done
  x = 2
  */
  MAKE_STATEMENTS(
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 1 ) ) ),
               makeBlock() ),
      MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), UnknownIdentifierException );
}

TEST_F( InterpreterIfStmtTest, can_mutate_outer_variables ) {
  /*
  var int x = 1
  if (true) do
      x = 2
  done
  */
  MAKE_STATEMENTS( MAKE_VAR_INT( "x", 1 ),
                   MAKE_IF( makeExprBlockPairVec(
                                MAKE_LITERAL( BaseType::BOOL, true ),
                                makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ) ) ),
                            makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( []( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const IfStatementNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      Variable compr{ "x", BaseType::INT, Mutability::MUTABLE, std::make_shared<Value>( 2 ) };
      ASSERT_EQ( compr, ITF::varsGlob( IT )[0] );
    }
  } );
  IT.execute();
}

TEST_F( InterpreterIfStmtTest, scope_decl_not_pollute_global ) {
  /*
  if (true) do
      int x = 1
  done
  */
  MAKE_STATEMENTS(
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock( MAKE_CONST_INT( "x", 1 ) ) ),
               makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const IfStatementNode*>( &node ) ) {
      // same state expected before and after
      assertCallStackSize( IT, 1u );
      assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
      assertTopCallContextVarCount( IT, 0u );
    }
  } );
  IT.execute();
}

TEST_F( InterpreterIfStmtTest, void_left_in_acc ) {
  /*
  if (true) do
  done
  */
  MAKE_STATEMENTS( MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ), makeBlock() ), makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const IfStatementNode*>( &node ) ) {
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
