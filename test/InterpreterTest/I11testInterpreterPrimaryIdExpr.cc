#include <gtest/gtest.h>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"  // IWYU pragma: keep
#include "Interpreter/Interpreter.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterPrimaryIdTest : public InterpreterTest {};

TEST_F( InterpreterPrimaryIdTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterPrimaryIdTest, local_scope_access ) {
  // var int x = 10
  // var int y = x
  auto x_decl = MAKE_DECL_VAR( "x", BaseType::INT, MAKE_LITERAL( BaseType::INT, 10 ) );
  auto y_decl = MAKE_DECL_VAR( "y", BaseType::INT, MAKE_ID( "x" ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *x_decl );
  IT.visit( *y_decl );

  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "y" ).value().get().getValue() ), 10 );
}

TEST_F( InterpreterPrimaryIdTest, nested_scope_sees_outer ) {
  // var int x = 42
  // var int y = 0
  // if (true) do
  //    y = x
  // done
  auto x_decl = MAKE_DECL_VAR( "x", BaseType::INT, MAKE_LITERAL( BaseType::INT, 42 ) );
  auto y_decl = MAKE_DECL_VAR( "y", BaseType::INT, MAKE_LITERAL( BaseType::INT, 0 ) );

  auto if_stmt = MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ),
                                                makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_ID( "x" ) ) ) ),
                          makeBlock() );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *x_decl );
  IT.visit( *y_decl );
  IT.visit( *if_stmt );

  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "y" ).value().get().getValue() ), 42 );
}

TEST_F( InterpreterPrimaryIdTest, nested_scope_shadowing ) {
  // var int x = 1
  // var int y = 0
  // if (true) do
  //    int x = 2
  //    y = x
  // done
  auto x_outer = MAKE_DECL_VAR( "x", BaseType::INT, MAKE_LITERAL( BaseType::INT, 1 ) );
  auto y_decl = MAKE_DECL_VAR( "y", BaseType::INT, MAKE_LITERAL( BaseType::INT, 0 ) );

  auto if_stmt =
      MAKE_IF( makeExprBlockPairVec( MAKE_LITERAL( BaseType::BOOL, true ),
                                     makeBlock( MAKE_DECL_CONST( "x", BaseType::INT, MAKE_LITERAL( BaseType::INT, 2 ) ),
                                                MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_ID( "x" ) ) ) ),
               makeBlock() );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *x_outer );
  IT.visit( *y_decl );
  IT.visit( *if_stmt );

  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), 1 );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "y" ).value().get().getValue() ), 2 );
}

TEST_F( InterpreterPrimaryIdTest, function_body_sees_global ) {
  // var int x = 100
  // var int y = 0
  // def void foo() do
  //    y = x
  // done
  // foo() <-man
  auto x_global = MAKE_DECL_VAR( "x", BaseType::INT, MAKE_LITERAL( BaseType::INT, 100 ) );
  auto y_global = MAKE_DECL_VAR( "y", BaseType::INT, MAKE_LITERAL( BaseType::INT, 0 ) );

  auto func_def =
      std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(),
                                         makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_ID( "x" ) ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  //   ITF::funcs( IT ).push_back( *func_def );
  IT.visit( *x_global );
  IT.visit( *y_global );
  IT.visit( *func_def );

  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "y" ).value().get().getValue() ), 100 );
}

TEST_F( InterpreterPrimaryIdTest, function_body_doesnt_see_call_scope ) {
  // def void foo() do
  //    x = 100
  // done
  // if (true) do
  //    int x = 55
  //    foo()
  // done
  auto y_global = MAKE_DECL_VAR( "y", BaseType::INT, MAKE_LITERAL( BaseType::INT, 0 ) );

  auto func_def =
      std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(),
                                         makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_ID( "x" ) ) ) );

  auto if_stmt =
      MAKE_IF( makeExprBlockPairVec(
                   MAKE_LITERAL( BaseType::BOOL, true ),
                   makeBlock( MAKE_DECL_VAR( "x", BaseType::INT, MAKE_LITERAL( BaseType::INT, 55 ) ),
                              std::make_unique<FunctionCallNode>( Position{ 1, 1 }, "foo", makeExpressions() ) ) ),
               makeBlock() );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func_def );
  IT.visit( *y_global );
  ASSERT_THROW( IT.visit( *if_stmt ), UnknownIdentifierException );
}