#include <gtest/gtest.h>

#include <cassert>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterFunctionDefTest : public InterpreterTest {};

TEST_F( InterpreterFunctionDefTest, check_init ) {
  ASSERT_TRUE( true );
}

/* @WARNING DECL PARAM RVALUE


*/

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_copy_from_rvalue ) {
  /*
  def void foo(copy int x) do
    return
  done
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl =
             dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {  // count enters only
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  ITF::addMockCallArg( IT, 1 );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_mutable_reference_from_rvalue_fail ) {
  /*
  def void foo(mut int x) do
    return
  done
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::addMockCallArg( IT, 1 );
  ASSERT_THROW( IT.visit( *func ), FunctionSignatureMismatchException );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_immutable_reference_from_rvalue_fail ) {
  /*
  def void foo(int x) do
    return
  done
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::addMockCallArg( IT, 1 );
  ASSERT_THROW( IT.visit( *func ), FunctionSignatureMismatchException );
}

/* @WARNING DECL PARAM LVALUE


*/

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_copy_from_immutable_lvalue ) {
  /*
  int x = 1
  def void foo(copy int x) do
    return
  done
  */
  auto decl = MAKE_CONST_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl =
             dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {  // count enters only
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_copy_from_mutable_lvalue ) {
  /*
  var int x = 1
  def void foo(copy int x) do
    return
  done
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl =
             dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {  // count enters only
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_immutable_reference_from_immutable_lvalue ) {
  /*
  int x = 1
  def void foo(int x) do
    return
  done
  */
  auto decl = MAKE_CONST_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl =
             dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {  // count enters only
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_mutable_reference_from_mutable_lvalue ) {
  /*
  var int x = 1
  def void foo(mut int x) do
    return
  done
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl =
             dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {  // count enters only
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::MUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_mutable_reference_from_immutable_lvalue_fail ) {
  /*
  int x = 1
  def void foo(mut int x) do
    return
  done
  foo(x[0])
  */
  auto decl = MAKE_CONST_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  IT.visit( *decl );
  IT.visit( *ref );
  ASSERT_THROW( IT.visit( *func ), FunctionSignatureMismatchException );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_immutable_reference_from_mutable_lvalue ) {
  /*
  var int x = 1
  def void foo(int x) do
    return
  done
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

/* @WARNING DECL PARAM  IREF


*/

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_copy_from_mutable_iref ) {
  /*
  var int[] x = [1,2,3]
  def void foo(copy int x) do
    return
  done
  foo(x[0])
  */
  auto decl = MAKE_DECL_VAR( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                             MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                               MAKE_LITERAL( BaseType::INT, 3 ) ) );
  auto ref = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl = dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_copy_from_immutable_iref ) {
  /*
  int[] x = [1,2,3]
  def void foo(copy int x) do
    return
  done
  foo(x[0])
  */
  auto decl = MAKE_DECL_CONST( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                               MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ) );
  auto ref = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl = dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_immutable_reference_from_immutable_iref ) {
  /*
  int[] x = [1,2,3]
  def void foo(int x) do
    return
  done
  foo(x[0])
  */
  auto decl = MAKE_DECL_CONST( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                               MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ) );
  auto ref = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl = dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_immutable_reference_from_mutable_iref ) {
  /*
  var int[] x = [1,2,3]
  def void foo(int x) do
    return
  done
  foo(x[0])
  */
  auto decl = MAKE_DECL_VAR( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                             MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                               MAKE_LITERAL( BaseType::INT, 3 ) ) );
  auto ref = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( auto decl = dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_mutable_reference_from_immutable_iref_fail ) {
  /*
  int[] x = [1,2,3]
  def void foo(mut int x) do
    return
  done
  foo(x[0])
  */
  auto decl = MAKE_DECL_CONST( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                               MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ) );
  auto ref = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  IT.visit( *decl );
  IT.visit( *ref );
  ASSERT_THROW( IT.visit( *func ), FunctionSignatureMismatchException );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_copy_from_rvalue_iref ) {
  /*
  def void foo(copy int x) do
    return
  done
  foo([1, 2, 3][0])
  */
  auto ref = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ),
                               MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int var_decl_counter = 0;
  IT.setDebugHook( [&var_decl_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto& var = ITF::env( IT ).getVarByNameThisScopeOnly( "x" ).value().get();
      Variable var_comp{ "x", BaseType::INT, Mutability::IMMUTABLE, std::make_shared<Value>( 1 ) };
      ASSERT_EQ( var, var_comp );
      ++var_decl_counter;
    }
  } );
  IT.visit( *ref );
  IT.visit( *func );
  ASSERT_EQ( 1, var_decl_counter );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_immutable_reference_from_rvalue_iref_fail ) {
  /*
  def void foo(int x) do
    return
  done
  foo([1, 2, 3][0])
  */
  auto ref = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ),
                               MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  IT.visit( *ref );
  ASSERT_THROW( IT.visit( *func ), FunctionSignatureMismatchException );
}

TEST_F( InterpreterFunctionDefTest, declare_param_as_var_mutable_reference_from_rvalue_iref_fail ) {
  /*
  def void foo(mut int x) do
    return
  done
  foo([1, 2, 3][0])
  */
  auto ref = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ),
                               MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  IT.visit( *ref );
  ASSERT_THROW( IT.visit( *func ), FunctionSignatureMismatchException );
}

/* @WARNING SHADOWING



*/

TEST_F( InterpreterFunctionDefTest, param_shadows_global_variable_is_allowed ) {
  /*
  var int x = 100
  def void foo(int x, int y, int z) do
    return
  done
  */
  auto global_decl = MAKE_VAR_INT( "x", 100 );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE },
                  ParameterDecl{ "y", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE },
                  ParameterDecl{ "z", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  int return_node_counter = 0;
  IT.setDebugHook( [&return_node_counter]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      auto local_x = ITF::env( interpreter ).getVarByNameThisScopeOnly( "x" );
      auto local_y = ITF::env( interpreter ).getVarByNameThisScopeOnly( "y" );
      auto local_z = ITF::env( interpreter ).getVarByNameThisScopeOnly( "z" );

      ASSERT_EQ( *( local_x.value().get().getValue() ), 100 );
      ASSERT_EQ( *( local_y.value().get().getValue() ), 20 );
      ASSERT_EQ( *( local_z.value().get().getValue() ), 30 );

      ++return_node_counter;
    }
  } );

  IT.visit( *global_decl );

  auto ref_x = MAKE_ID( "x" );
  IT.visit( *ref_x );
  ITF::addMockCallArg( IT, 20 );
  ITF::addMockCallArg( IT, 30 );

  IT.visit( *func );

  ASSERT_EQ( 1, return_node_counter );
}

TEST_F( InterpreterFunctionDefTest, duplicate_params_two_arguments_throws_exception ) {
  /*
  def void bar(int x, int x) do
    return
  done
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "bar", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE },
                  ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  ITF::addMockCallArg( IT, 10 );
  ITF::addMockCallArg( IT, 20 );

  ASSERT_THROW( IT.visit( *func ), InvalidShadowException );
}

TEST_F( InterpreterFunctionDefTest, duplicate_params_three_arguments_throws_exception ) {
  /*
  def void baz(int x, int y, int x) do
    return
  done
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "baz", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE },
                  ParameterDecl{ "y", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE },
                  ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  ITF::addMockCallArg( IT, 10 );
  ITF::addMockCallArg( IT, 20 );
  ITF::addMockCallArg( IT, 30 );

  ASSERT_THROW( IT.visit( *func ), InvalidShadowException );
}

TEST_F( InterpreterFunctionDefTest, mutable_reference_parameter_mutates_referenced_variable ) {
  /*
  var int x = 100
  def void modify(ref int y) do
    y = 200
    return
  done
  */
  auto global_decl = MAKE_VAR_INT( "x", 100 );

  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "modify", BaseType::VOID,
      makeParams( ParameterDecl{ "y", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_LITERAL( BaseType::INT, 200 ) ), MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *global_decl );
  auto ref_x = MAKE_ID( "x" );
  IT.visit( *ref_x );
  IT.visit( *func );
  auto global_x = ITF::env( IT ).getVarByNameThisScopeOnly( "x" );
  ASSERT_TRUE( global_x.has_value() );
  ASSERT_EQ( *( global_x.value().get().getValue() ), 200 );
}

TEST_F( InterpreterFunctionDefTest, mutable_reference_parameter_with_same_name_mutates_referenced_variable ) {
  /*
  var int x = 100
  def void modify(ref int x) do
    x = 500
    return
  done
  */
  auto global_decl = MAKE_VAR_INT( "x", 100 );

  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "modify", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 500 ) ), MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  IT.visit( *global_decl );
  auto ref_x = MAKE_ID( "x" );
  IT.visit( *ref_x );
  IT.visit( *func );
  auto global_x = ITF::env( IT ).getVarByNameThisScopeOnly( "x" );
  ASSERT_TRUE( global_x.has_value() );
  ASSERT_EQ( *( global_x.value().get().getValue() ), 500 );
}

TEST_F( InterpreterFunctionDefTest, modifying_immutable_copy_parameter_throws_exception ) {
  /*
  var int x = 100
  def void try_modify(int y) do  # IMMUTABLE copy
    y = 200
    return
  done
  */
  auto global_decl = MAKE_VAR_INT( "x", 100 );

  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "try_modify", BaseType::VOID,
      makeParams( ParameterDecl{ "y", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_ASSIGNMENT_EXPR( MAKE_ID( "y" ), MAKE_LITERAL( BaseType::INT, 200 ) ), MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *global_decl );
  auto ref_x = MAKE_ID( "x" );
  IT.visit( *ref_x );
  ASSERT_THROW( IT.visit( *func ), InvalidAccessException );
}

TEST_F( InterpreterFunctionDefTest, parameters_are_cleared_after_function_ends ) {
  /*
  def void foo(int param_x) do
    return
  done
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "param_x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  bool param_existed_during_run = false;
  IT.setDebugHook( [&param_existed_during_run]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    if ( dynamic_cast<const ReturnNode*>( &node ) && event == DebugEvent::AFTER_NODE_VISIT ) {
      if ( ITF::env( interpreter ).getVarByNameThisScopeOnly( "param_x" ).has_value() ) {
        param_existed_during_run = true;
      }
    }
  } );
  ITF::addMockCallArg( IT, 1 );
  IT.visit( *func );
  ASSERT_TRUE( param_existed_during_run );
  auto param_after_execution = ITF::env( IT ).getVarByNameThisScopeOnly( "param_x" );
  ASSERT_FALSE( param_after_execution.has_value() );
}

/* @WARNING RETURN


*/

TEST_F( InterpreterFunctionDefTest, void_function_pushes_void_to_acc ) {
  /*
  var int x = 1
  def void foo(int x) do
    return
  done
  foo(x)
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto ref = MAKE_ID( "x" );
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( nullptr ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  IT.visit( *ref );
  IT.visit( *func );

  assertAccTopVal( IT, true, 0 );
}

TEST_F( InterpreterFunctionDefTest, void_function_ends_naturally_without_return_pushes_void_to_acc ) {
  /*
  def void foo() do
  done
  foo()
  */
  auto func = std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(), makeBlock() );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *func );
  assertAccTopVal( IT, true, 0 );
}

TEST_F( InterpreterFunctionDefTest, void_function_fails_if_non_void_returned ) {
  /*
  def void foo() do
    return 42
  done
  foo()
  */
  auto func = std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(),
                                                 makeBlock( MAKE_RETURN( MAKE_LITERAL( BaseType::INT, 42 ) ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_THROW( IT.visit( *func ), VoidValueException );
}

TEST_F( InterpreterFunctionDefTest, paramless_non_void_function_returns_matching_value ) {
  /*
  def int foo() do
    return 42
  done
  foo()
  */
  auto func = std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::INT, makeParams(),
                                                 makeBlock( MAKE_RETURN( MAKE_LITERAL( BaseType::INT, 42 ) ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *func );
  assertAccTopVal( IT, false, 42 );
}

TEST_F( InterpreterFunctionDefTest, non_void_function_fails_if_void_returned ) {
  /*
  def int foo() do
    return
  done
  foo()
  */
  auto func = std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::INT, makeParams(),
                                                 makeBlock( MAKE_RETURN( nullptr ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_THROW( IT.visit( *func ), VoidValueException );
}

TEST_F( InterpreterFunctionDefTest, non_void_function_fails_if_mismatched_type_returned ) {
  /*
  def int foo() do
    return true
  done
  foo()
  */
  auto func = std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::INT, makeParams(),
                                                 makeBlock( MAKE_RETURN( MAKE_LITERAL( BaseType::BOOL, true ) ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_THROW( IT.visit( *func ), NotAllowedTypeException );
}

TEST_F( InterpreterFunctionDefTest, non_void_function_fails_if_no_return ) {
  /*
  def int foo() do
  done
  foo()
  */
  auto func = std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::INT, makeParams(), makeBlock() );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_THROW( IT.visit( *func ), VoidValueException );
}

TEST_F( InterpreterFunctionDefTest, return_terminates_function_immediately ) {
  /*
  def void foo() do
    return
    return 42
  done
  foo()
  */

  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(),
      makeBlock( MAKE_RETURN( nullptr ), MAKE_RETURN( MAKE_LITERAL( BaseType::INT, 42 ) ) ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_NO_THROW( IT.visit( *func ) );
  assertAccTopVal( IT, true, 0 );
}