#include <gtest/gtest.h>

#include <cassert>
#include <optional>

#include "Interpreter/Interpreter.h"
#include "Interpreter/RuntimeValue.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

TEST_F( InterpreterTest, check_init3 ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterTest, check_debug_guard ) {
  Interpreter interpreter{ nullptr };  // @TODO make interpreter builder
  interpreter.setDebugHook( [skips = 0]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    switch ( event ) {
      case DebugEvent::BEFORE_NODE_VISIT:
      case DebugEvent::AFTER_NODE_VISIT:
    }
  } );
}

TEST_F( InterpreterTest, ctor_adds_builtin_functions ) {
  std::vector<std::unique_ptr<INode>> statements;
  std::vector<std::unique_ptr<FunctionDefNode>> functions;
  Interpreter interpreter{
      std::make_unique<ProgramNode>( Position{ 1, 1 }, std::move( statements ), std::move( functions ) ) };
  ASSERT_EQ( 2u, ITF::funcs( interpreter ).size() );
  {
    std::vector<ParameterDecl> params_read;
    params_read.push_back( ParameterDecl{"prompt", Type::buildTypeArrayTypeFromBase(BaseType::CHAR), PassMode::COPY, Mutability::IMMUTABLE} );
    ASSERT_NE( std::nullopt, ITF::env( interpreter ).getFunctionBySignature( "read",params_read ) );
  }
  {
    std::vector<RuntimeValue> vars_exit;
    vars_exit.push_back( RuntimeValue{ Value{ 1 } } );
    ASSERT_NE( std::nullopt, ITF::env( interpreter ).getFunctionBySignature( "exit", vars_exit ) );
  }
}

TEST_F( InterpreterTest, register_function ) {
  /*
  def void foo() do
  done
  */

  std::vector<std::unique_ptr<INode>> statements;
  std::vector<std::unique_ptr<FunctionDefNode>> functions;
  functions.push_back( std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID,
                                                          std::vector<ParameterDecl>(), Block() ) );
  Interpreter interpreter{
      std::make_unique<ProgramNode>( Position{ 1, 1 }, std::move( statements ), std::move( functions ) ) };

  ASSERT_EQ( ITF::funcs( interpreter ).size(), 3 );
}