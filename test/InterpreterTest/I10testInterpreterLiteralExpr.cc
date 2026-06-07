#include <gtest/gtest.h>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"  // IWYU pragma: keep
#include "Interpreter/Interpreter.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterLiteralExprTest : public InterpreterTest {};

TEST_F( InterpreterLiteralExprTest, check_init ) {
  ASSERT_TRUE( true );
}

/* int


*/

TEST_F( InterpreterLiteralExprTest, int_positive ) {
  auto expr{ MAKE_LITERAL( BaseType::INT, 42 ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 42 );
}

TEST_F( InterpreterLiteralExprTest, int_negative ) {
  auto expr{ MAKE_LITERAL( BaseType::INT, -1337 ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, -1337 );
}

TEST_F( InterpreterLiteralExprTest, int_zero ) {
  auto expr{ MAKE_LITERAL( BaseType::INT, 0 ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 0 );
}

/* float


*/

TEST_F( InterpreterLiteralExprTest, float_positive ) {
  auto expr{ MAKE_LITERAL( BaseType::FLOAT, 3.1415f ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 3.1415f );
}

TEST_F( InterpreterLiteralExprTest, float_negative ) {
  auto expr{ MAKE_LITERAL( BaseType::FLOAT, -0.005f ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, -0.005f );
}

TEST_F( InterpreterLiteralExprTest, float_zero ) {
  auto expr{ MAKE_LITERAL( BaseType::FLOAT, 0.0f ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 0.0f );
}

/* char



*/

TEST_F( InterpreterLiteralExprTest, char_alphabetic ) {
  auto expr{ MAKE_LITERAL( BaseType::CHAR, 'A' ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 'A' );
}

TEST_F( InterpreterLiteralExprTest, char_numeric ) {
  auto expr{ MAKE_LITERAL( BaseType::CHAR, '7' ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, '7' );
}

TEST_F( InterpreterLiteralExprTest, char_escape_sequence ) {
  auto expr{ MAKE_LITERAL( BaseType::CHAR, '\n' ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, '\n' );
}

/* bool



*/

TEST_F( InterpreterLiteralExprTest, bool_true ) {
  auto expr{ MAKE_LITERAL( BaseType::BOOL, true ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, true );
}

TEST_F( InterpreterLiteralExprTest, bool_false ) {
  auto expr{ MAKE_LITERAL( BaseType::BOOL, false ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, false );
}

/* arrays



*/

TEST_F( InterpreterLiteralExprTest, arr_int_single_element ) {
  auto expr{ MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 99 ) ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 99 ) );
}

TEST_F( InterpreterLiteralExprTest, arr_float_multiple_elements ) {
  auto expr{ MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::FLOAT, -1.1f ), MAKE_LITERAL( BaseType::FLOAT, 0.0f ),
                               MAKE_LITERAL( BaseType::FLOAT, 1.1f ) ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( -1.1f, 0.0f, 1.1f ) );
}

TEST_F( InterpreterLiteralExprTest, arr_bool_alternating ) {
  auto expr{ MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::BOOL, true ), MAKE_LITERAL( BaseType::BOOL, false ),
                               MAKE_LITERAL( BaseType::BOOL, true ) ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( true, false, true ) );
}

TEST_F( InterpreterLiteralExprTest, arr_empty ) {
  auto expr{ MAKE_ARR_LITERAL() };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterLiteralExprTest, string_empty_char_array ) {
  auto expr{ MAKE_ARR_LITERAL() };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterLiteralExprTest, string_populated_char_array ) {
  auto expr{ MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::CHAR, 'H' ), MAKE_LITERAL( BaseType::CHAR, 'i' ) ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 'H', 'i' ) );
}

/* nested arrays



*/

TEST_F( InterpreterLiteralExprTest, arr_nested_int_matrix ) {
  // [[1, 2], [3, 4]]
  auto expr{
      MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 4 ) ) ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( Value::makeArray( 1, 2 ), Value::makeArray( 3, 4 ) ) );
}