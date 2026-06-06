#include <gtest/gtest.h>

#include <cassert>
#include <optional>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterVarDeclTest : public InterpreterTest {};

TEST_F( InterpreterVarDeclTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterVarDeclTest, const_added_to_cur_call_context_vars ) {
  // <d>int x = 1</d>
  /*    var_decl
            - lhs: primary_identifier: "x"
            - rhs: literal_expr: 1
  */
  MAKE_STATEMENTS( MAKE_CONST_INT( "x", 1 ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 0u );
          assertAccSize( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 1u );
          assertAccSize( IT, 1u );
          ASSERT_TRUE( ITF::acc( IT ).top().isVoid() && "statement call ought to leave void in acc" );
          Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterVarDeclTest, var_added_to_cur_call_context_vars ) {
  // <d>var int x = 1</d>
  /*    var_decl
            - lhs: primary_identifier: "x"
            - rhs: literal_expr: 1
  */

  MAKE_STATEMENTS( MAKE_VAR_INT( "x", 1 ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 0u );
          assertAccSize( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 1u );
          assertAccSize( IT, 1u );
          ASSERT_TRUE( ITF::acc( IT ).top().isVoid() && "statement call ought to leave void in acc" );
          Variable var_comp{ "x", BaseType::INT, Mutability::MUTABLE, std::make_shared<Value>( 1 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterVarDeclTest, shadowing_not_allowed_in_same_scope ) {
  // int x = 1
  // int x = 2
  /*    var_decl
            - lhs: primary_identifier: "x"
            - rhs: literal_expr: 1
        var_decl
            - lhs: primary_identifier: "x"
            - rhs: literal_expr: 2
  */

  MAKE_STATEMENTS( MAKE_CONST_INT( "x", 1 ), MAKE_CONST_INT( "x", 2 ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), InvalidShadowException );
}

TEST_F( InterpreterVarDeclTest, error_when_init_expr_is_void ) {
  // var int x = 1
  // int y = x = 1
  /*    var_decL
            - lhs: primary identifier: "x"
            - rhs: literal: 1
        var_decl
            - lhs: primary_identifier: "y"
            - rhs(initializer expr): assignment:
                                      - lhs: primary_identifier: "y"
                                      - rhs: literal: 1
    x = 1 -> returns void
  */
  MAKE_STATEMENTS(
      MAKE_VAR_INT( "x", 1 ),
      MAKE_DECL_CONST( "y", BaseType::INT, MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 1 ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), VoidValueException );
}

TEST_F( InterpreterVarDeclTest, shadow_fun_param ) {
  /*
  void foo(copy int x) do
      int x = 1
  done
  foo(1)
  */
  MAKE_STATEMENTS( std::make_unique<FunctionCallNode>( Position{ 1, 1 }, "foo",
                                                       makeExpressions( MAKE_LITERAL( BaseType::INT, 1 ) ) ) );
  MAKE_FUNCTIONS( std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_CONST_INT( "x", 1 ) ) ) );
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), InvalidShadowException );
}

TEST_F( InterpreterVarDeclTest, fun_var_shadows_global_var ) {
  /*
  int x = 1
  void foo() do
      <d>int x = 1</d>
  done
  foo()
  */
  MAKE_STATEMENTS( MAKE_CONST_INT( "x", 1 ),
                   std::make_unique<FunctionCallNode>( Position{ 1, 1 }, "foo", makeExpressions() ) );
  MAKE_FUNCTIONS( std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(),
                                                     makeBlock( MAKE_CONST_INT( "x", 1 ) ) ) );
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 1]( Interpreter& interpreter, const INode& node, DebugEvent event ) mutable {
    if ( skips > 0 ) {
      --skips;
      return;
    }
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 2u );
          assertTopCallContextType( IT, CallContext::ContextType::FUNCTION_CALL );
          assertTopCallContextVarCount( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 2u );
          assertTopCallContextType( IT, CallContext::ContextType::FUNCTION_CALL );
          assertTopCallContextVarCount( IT, 1u );
          Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          ASSERT_EQ( var_comp, ITF::varsGlob( IT )[0] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterVarDeclTest, scoped_var_shadows_scope_below ) {
  /*
  if (true) do
      int x = 1
      <d2>if (true) do
          <d1>int x = 2<d1>
      done</d2>
  done
  */
  MAKE_STATEMENTS( MAKE_IF(
      makeExprBlockPairVec(
          MAKE_LITERAL( BaseType::BOOL, true ),
          makeBlock( MAKE_CONST_INT( "x", 1 ), MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ),
                                                                              makeBlock( MAKE_CONST_INT( "x", 2 ) ) ),
                                                        makeBlock() ) ) ),
      makeBlock() ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 1]( Interpreter& interpreter, const INode& node, DebugEvent event ) mutable {
    if ( skips > 0 ) {
      --skips;
      return;
    }
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 3u );
          assertTopCallContextType( IT, CallContext::ContextType::IF_BLOCK );
          assertTopCallContextVarCount( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 3u );
          assertTopCallContextType( IT, CallContext::ContextType::FUNCTION_CALL );
          assertTopCallContextVarCount( IT, 1u );
          Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 2 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          break;
        }
      }
    }
    if ( dynamic_cast<const IfStatementNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 3u );
          assertTopCallContextType( IT, CallContext::ContextType::IF_BLOCK );
          assertTopCallContextVarCount( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 3u );
          assertTopCallContextType( IT, CallContext::ContextType::FUNCTION_CALL );
          assertTopCallContextVarCount( IT, 1u );
          Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 2 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          Variable var_comp_underneath{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
          ASSERT_EQ( var_comp_underneath, ITF::callStack( IT ).nth( 1u ).value().get().getVariables()[0u] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterVarDeclTest, char_arr_init_by_string_literal ) {
  // <d>char[] x = "abc"</d>
  /*    var_decl
            - lhs: primary_identifier: "x"
            - rhs: literal_expr: "abc"
  */
  MAKE_STATEMENTS( MAKE_DECL_CONST(
      "x", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
      MAKE_LITERAL( Type::buildTypeArrayTypeFromBase( BaseType::CHAR ), Value::makeArray( 'a', 'b', 'c' ) ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const VarOrConstDeclNode*>( &node ) ) {
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 0u );
          assertAccSize( IT, 0u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 1u );
          assertAccSize( IT, 1u );
          ASSERT_TRUE( ITF::acc( IT ).top().isVoid() && "statement call ought to leave void in acc" );
          Variable var_comp{ "x", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ), Mutability::IMMUTABLE,
                             std::make_shared<Value>( Value::makeArray( 'a', 'b', 'c' ) ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[0] );
          break;
        }
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterVarDeclTest, type_mismatch ) {
  // int x = 'c'
  /*    var_decl
            - lhs: primary_identifier: "x"
            - rhs: literal_expr: 'a'
  */
  MAKE_STATEMENTS( MAKE_DECL_CONST( "x", BaseType::INT, MAKE_LITERAL( BaseType::CHAR, 'c' ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), NotAllowedTypeException );
}

TEST_F( InterpreterVarDeclTest, var_inaccessible_during_initialization ) {
  /*
  var int x = x + 1
  */
  MAKE_STATEMENTS( MAKE_DECL_VAR(
      "x", BaseType::INT, MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::ADD ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), UnknownIdentifierException );
}

TEST_F( InterpreterVarDeclTest, init_by_read_makes_unrelated_val ) {
  /*
  var int x = 1
  var int y = x
  <d>x = 2<d> // <- y must remain 1
  */
  MAKE_STATEMENTS( MAKE_VAR_INT( "x", 1 ), MAKE_DECL_VAR( "y", BaseType::INT, MAKE_ID( "x" ) ),
                   MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) mutable {
    if ( dynamic_cast<const AssignmentExprNode*>( &node ) ) {  // just to stop the program, don't care about type
      switch ( event ) {
        case DebugEvent::BEFORE_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 2u );
          break;
        }
        case DebugEvent::AFTER_NODE_VISIT: {
          assertCallStackSize( IT, 1u );
          assertTopCallContextType( IT, CallContext::ContextType::TOP_LEVEL );
          assertTopCallContextVarCount( IT, 2u );
          Variable var_comp{ "y", BaseType::INT, Mutability::MUTABLE, std::make_shared<Value>( 1 ) };
          ASSERT_EQ( var_comp, ITF::varsTop( IT )[1] );
          break;
        }
      }
    }
  } );
  IT.execute();
}