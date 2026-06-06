#include <gtest/gtest.h>

#include <cassert>


#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/Interpreter.h"
#include "TestHelperInterpreter.hpp"

class InterpreterFunctionDefTest : public InterpreterTest {};

TEST_F( InterpreterFunctionDefTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterFunctionDefTest, check_debug_guard ) {
  MAKE_STATEMENTS();
  MAKE_FUNCTIONS();
  MAKE_INTERPRETER  //@TODO cont
      interpreter.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
        if ( isIgnoredType<BuiltinFunction>( node ) ) {
          return;
        }
        switch ( event ) {
          case DebugEvent::BEFORE_NODE_VISIT:
          case DebugEvent::AFTER_NODE_VISIT:
        }
      } );
}