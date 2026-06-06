#include <gtest/gtest.h>

#include <cassert>
#include <optional>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

TEST_F( InterpreterTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterTest, ctor_adds_builtin_functions ) {
  MAKE_STATEMENTS();
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER
  ASSERT_EQ( 2u, ITF::funcs( IT ).size() );
  {
    auto params_read{ makeParams( ParameterDecl{ "prompt", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
                                                 PassMode::COPY, Mutability::IMMUTABLE } ) };
    ASSERT_NE( std::nullopt, ITF::env( IT ).getFunctionBySignature( "read", params_read ) );
  }
  {
    auto params_exit{ makeParams( ParameterDecl{ "code", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ) };
    ASSERT_NE( std::nullopt, ITF::env( IT ).getFunctionBySignature( "exit", params_exit ) );
  }
}

TEST_F( InterpreterTest, register_function ) {
  /*
  def void foo() do
  done
  */
  MAKE_STATEMENTS();
  MAKE_FUNCTIONS(
      std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(), makeBlock() ) );
  MAKE_INTERPRETER
  IT.execute();
  ASSERT_EQ( ITF::funcs( IT ).size(), 3 );
}

TEST_F( InterpreterTest, call_stack_empty_after_execute_from_empty ) {
  MAKE_STATEMENTS();
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  IT.setDebugHook( [skips = 0]( Interpreter& IT, const INode& node, DebugEvent event ) {
    if ( !dynamic_cast<const ProgramNode*>( &node ) ) {
      return;
    }
    switch ( event ) {
      case DebugEvent::BEFORE_NODE_VISIT: {
        ASSERT_TRUE( ITF::callStack( IT ).empty() );
        break;
      }
      case DebugEvent::AFTER_NODE_VISIT: {
        ASSERT_TRUE( ITF::callStack( IT ).empty() );
        break;
      }
    }
  } );
  IT.execute();
}

TEST_F( InterpreterTest, polluting_statement_illegal ) {
  // 1 <- puts RuntimeValue{Value{1}} in acc
  MAKE_STATEMENTS( MAKE_LITERAL( BaseType::INT, 1 ) );
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER

  ASSERT_THROW( IT.execute(), InvalidStatementException );
}

// dropping context and all