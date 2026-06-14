#include <gtest/gtest.h>

#include <cassert>
#include <vector>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

std::ostream& operator<<( std::ostream& os, const Value& value ) {
  return os << std::format( "{}", value );
}

class InterpreterMapFilterTest : public InterpreterTest {};

/* @WARNING MAP OPERATOR - HAPPY PATHS */

TEST_F( InterpreterMapFilterTest, map_standard_execution_base_to_base ) {
  /*
  def int makeDouble(copy int x) do
    return x * 2
  done
  var int[] arr = [1, 2, 3]
  var int[] res = arr -> makeDouble
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "makeDouble", BaseType::INT,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock(
          MAKE_RETURN( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ), BinaryOperator::MUL ) ) ) );

  auto arr_decl = MAKE_DECL_VAR( "arr", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                                 MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                   MAKE_LITERAL( BaseType::INT, 3 ) ) );

  auto map_op = MAKE_ARR_OP_SPEC( MAKE_ID( "arr" ), ArrayIdentifierOpType::MAP, "makeDouble" );
  auto res_decl = MAKE_DECL_VAR( "res", Type::buildTypeArrayTypeFromBase( BaseType::INT ), std::move( map_op ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  ITF::funcs( IT ).push_back( *func );
  IT.visit( *arr_decl );
  ASSERT_NO_THROW( IT.visit( *res_decl ) );

  auto res_var = ITF::env( IT ).getVarByNameThisScopeOnly( "res" );
  ASSERT_TRUE( res_var.has_value() );
  ASSERT_EQ( *( res_var.value().get().getValue() ), Value::makeArray( 2, 4, 6 ) );
}

TEST_F( InterpreterMapFilterTest, map_type_transformation ) {
  /*
  def bool isPositive(copy int x) do
    return x > 0
  done
  var bool[] res = [-1, 1] -> isPositive
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "isPositive", BaseType::BOOL,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock(
          MAKE_RETURN( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::GT ) ) ) );

  auto arr_literal = MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, -1 ), MAKE_LITERAL( BaseType::INT, 1 ) );
  auto map_op = MAKE_ARR_OP_SPEC( std::move( arr_literal ), ArrayIdentifierOpType::MAP, "isPositive" );
  auto res_decl = MAKE_DECL_VAR( "res", Type::buildTypeArrayTypeFromBase( BaseType::BOOL ), std::move( map_op ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func );

  ASSERT_NO_THROW( IT.visit( *res_decl ) );

  auto res_var = ITF::env( IT ).getVarByNameThisScopeOnly( "res" );
  ASSERT_TRUE( res_var.has_value() );
  ASSERT_EQ( *( res_var.value().get().getValue() ), Value::makeArray( false, true ) );
}

/* @WARNING MAP OPERATOR - EXCEPTIONS & VALIDATION */

TEST_F( InterpreterMapFilterTest, map_exception_non_array_left_operand ) {
  /*
  def int makeDouble(copy int x) do return x * 2 done
  var int res = 5 -> makeDouble
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "makeDouble", BaseType::INT,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock(
          MAKE_RETURN( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ), BinaryOperator::MUL ) ) ) );

  auto map_op = MAKE_ARR_OP_SPEC( MAKE_LITERAL( BaseType::INT, 5 ), ArrayIdentifierOpType::MAP, "makeDouble" );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func );

  ASSERT_THROW( IT.visit( *map_op ), NotAllowedTypeException );
}

TEST_F( InterpreterMapFilterTest, map_exception_missing_function ) {
  /*
  var int[] arr = [1, 2]
  arr -> doesNotExist
  */
  auto arr_literal = MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) );
  auto map_op = MAKE_ARR_OP_SPEC( std::move( arr_literal ), ArrayIdentifierOpType::MAP, "doesNotExist" );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  ASSERT_THROW( IT.visit( *map_op ), InvalidAccessException );
}

TEST_F( InterpreterMapFilterTest, map_exception_pass_by_reference_rejected ) {
  /*
  def int mutate(mut int x) do return x done
  [1, 2] -> mutate
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "mutate", BaseType::INT,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::REFERENCE, Mutability::MUTABLE } ),
      makeBlock( MAKE_RETURN( MAKE_ID( "x" ) ) ) );

  auto arr_literal = MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) );
  auto map_op = MAKE_ARR_OP_SPEC( std::move( arr_literal ), ArrayIdentifierOpType::MAP, "mutate" );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func );

  ASSERT_THROW( IT.visit( *map_op ), InvalidAccessException );
}

/* @WARNING FILTER OPERATOR - HAPPY PATHS */

TEST_F( InterpreterMapFilterTest, filter_standard_execution_partial_match ) {
  /*
  def bool isGreaterThanOne(copy int x) do
    return x > 1
  done
  var int[] arr = [1, 2, 3]
  var int[] res = arr ? isGreaterThanOne
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "isGreaterThanOne", BaseType::BOOL,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock(
          MAKE_RETURN( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::GT ) ) ) );

  auto arr_decl = MAKE_DECL_VAR( "arr", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                                 MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                   MAKE_LITERAL( BaseType::INT, 3 ) ) );

  auto filter_op = MAKE_ARR_OP_SPEC( MAKE_ID( "arr" ), ArrayIdentifierOpType::FILTER, "isGreaterThanOne" );
  auto res_decl = MAKE_DECL_VAR( "res", Type::buildTypeArrayTypeFromBase( BaseType::INT ), std::move( filter_op ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  ITF::funcs( IT ).push_back( *func );
  IT.visit( *arr_decl );
  ASSERT_NO_THROW( IT.visit( *res_decl ) );

  auto res_var = ITF::env( IT ).getVarByNameThisScopeOnly( "res" );
  ASSERT_TRUE( res_var.has_value() );
  ASSERT_EQ( *( res_var.value().get().getValue() ), Value::makeArray( 2, 3 ) );
}

TEST_F( InterpreterMapFilterTest, filter_empty_array ) {
  /*
  def bool isEven(copy int x) do return true done
  var int[] arr = []
  var int[] res = arr ? isEven
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "isEven", BaseType::BOOL,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( MAKE_LITERAL( BaseType::BOOL, true ) ) ) );

  auto arr_decl = MAKE_DECL_VAR( "arr", Type::buildTypeArrayTypeFromBase( BaseType::INT ), MAKE_ARR_LITERAL() );
  auto filter_op = MAKE_ARR_OP_SPEC( MAKE_ID( "arr" ), ArrayIdentifierOpType::FILTER, "isEven" );
  auto res_decl = MAKE_DECL_VAR( "res", Type::buildTypeArrayTypeFromBase( BaseType::INT ), std::move( filter_op ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func );
  IT.visit( *arr_decl );
  ASSERT_NO_THROW( IT.visit( *res_decl ) );

  auto res_var = ITF::env( IT ).getVarByNameThisScopeOnly( "res" );
  ASSERT_TRUE( res_var.has_value() );
  ASSERT_EQ( *( res_var.value().get().getValue() ), Value::makeArray() );
}

/* @WARNING FILTER OPERATOR - EXCEPTIONS & VALIDATION */

TEST_F( InterpreterMapFilterTest, filter_exception_non_boolean_return_type ) {
  /*
  def int returnInt(copy int x) do return x done
  [1, 2] ? returnInt
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "returnInt", BaseType::INT,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( MAKE_ID( "x" ) ) ) );

  auto arr_literal = MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) );
  auto filter_op = MAKE_ARR_OP_SPEC( std::move( arr_literal ), ArrayIdentifierOpType::FILTER, "returnInt" );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func );

  ASSERT_THROW( IT.visit( *filter_op ), NotAllowedTypeException );
}

TEST_F( InterpreterMapFilterTest, filter_exception_parameter_count_mismatch ) {
  /*
  def bool twoArgs(copy int x, copy int y) do return true done
  [1, 2] ? twoArgs
  */
  auto func = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "twoArgs", BaseType::BOOL,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE },
                  ParameterDecl{ "y", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock( MAKE_RETURN( MAKE_LITERAL( BaseType::BOOL, true ) ) ) );

  auto arr_literal = MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) );
  auto filter_op = MAKE_ARR_OP_SPEC( std::move( arr_literal ), ArrayIdentifierOpType::FILTER, "twoArgs" );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ITF::funcs( IT ).push_back( *func );

  ASSERT_THROW( IT.visit( *filter_op ), InvalidAccessException );
}

/* @WARNING CHAINING OPERATORS */

TEST_F( InterpreterMapFilterTest, chained_filter_and_map ) {
  /*
  def bool isGreaterThanOne(copy int x) do return x > 1 done
  def int makeDouble(copy int x) do return x * 2 done

  var int[] arr = [1, 2, 3]
  var int[] res = arr ? isGreaterThanOne -> makeDouble
  */
  auto func_filter = std::make_unique<FunctionDefNode>(
      Position{ 1, 1 }, "isGreaterThanOne", BaseType::BOOL,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock(
          MAKE_RETURN( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::GT ) ) ) );

  auto func_map = std::make_unique<FunctionDefNode>(
      Position{ 2, 1 }, "makeDouble", BaseType::INT,
      makeParams( ParameterDecl{ "x", BaseType::INT, PassMode::COPY, Mutability::IMMUTABLE } ),
      makeBlock(
          MAKE_RETURN( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ), BinaryOperator::MUL ) ) ) );

  auto arr_decl = MAKE_DECL_VAR( "arr", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                                 MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                   MAKE_LITERAL( BaseType::INT, 3 ) ) );

  // AST: (arr ? isGreaterThanOne) -> makeDouble
  auto filter_op = MAKE_ARR_OP_SPEC( MAKE_ID( "arr" ), ArrayIdentifierOpType::FILTER, "isGreaterThanOne" );
  auto map_op = MAKE_ARR_OP_SPEC( std::move( filter_op ), ArrayIdentifierOpType::MAP, "makeDouble" );

  auto res_decl = MAKE_DECL_VAR( "res", Type::buildTypeArrayTypeFromBase( BaseType::INT ), std::move( map_op ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );

  ITF::funcs( IT ).push_back( *func_filter );
  ITF::funcs( IT ).push_back( *func_map );
  IT.visit( *arr_decl );

  ASSERT_NO_THROW( IT.visit( *res_decl ) );

  auto res_var = ITF::env( IT ).getVarByNameThisScopeOnly( "res" );
  ASSERT_TRUE( res_var.has_value() );
  ASSERT_EQ( *( res_var.value().get().getValue() ), Value::makeArray( 4, 6 ) );
}