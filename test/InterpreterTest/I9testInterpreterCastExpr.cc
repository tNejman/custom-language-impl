#include <gtest/gtest.h>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/Interpreter.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterCastExprTest : public InterpreterTest {};

TEST_F( InterpreterCastExprTest, check_init ) {
  ASSERT_TRUE( true );
}

/* Casting int

*/

TEST_F( InterpreterCastExprTest, int_to_int ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, 65 ), BaseType::INT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 65 );
}

TEST_F( InterpreterCastExprTest, int_to_float ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, 65 ), BaseType::FLOAT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 65.f );
}

TEST_F( InterpreterCastExprTest, int_to_char ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, 65 ), BaseType::CHAR ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 'A' );
}

TEST_F( InterpreterCastExprTest, int_to_bool_true ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, -5 ), BaseType::BOOL ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, true );
}

TEST_F( InterpreterCastExprTest, int_to_bool_false ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, 0 ), BaseType::BOOL ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, false );
}

/* casting float


*/

TEST_F( InterpreterCastExprTest, float_to_int ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::FLOAT, 12.34f ), BaseType::INT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 12 );
}

TEST_F( InterpreterCastExprTest, float_to_float ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::FLOAT, -1.5f ), BaseType::FLOAT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, -1.5f );
}

TEST_F( InterpreterCastExprTest, float_to_char ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::FLOAT, 65.0f ), BaseType::CHAR ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}

TEST_F( InterpreterCastExprTest, float_to_bool_true ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::FLOAT, 0.001f ), BaseType::BOOL ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, true );
}

TEST_F( InterpreterCastExprTest, float_to_bool_false ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::FLOAT, 0.0f ), BaseType::BOOL ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, false );
}

/* char

*/

TEST_F( InterpreterCastExprTest, char_to_int ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::CHAR, 'B' ), BaseType::INT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 66 );
}

TEST_F( InterpreterCastExprTest, char_to_float ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::CHAR, 'A' ), BaseType::FLOAT ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}

TEST_F( InterpreterCastExprTest, char_to_char ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::CHAR, 'z' ), BaseType::CHAR ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 'z' );
}

TEST_F( InterpreterCastExprTest, char_to_bool ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::CHAR, '\0' ), BaseType::BOOL ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}

/* bool


*/

TEST_F( InterpreterCastExprTest, bool_to_int ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), BaseType::INT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 1 );
}

TEST_F( InterpreterCastExprTest, bool_to_float ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::BOOL, false ), BaseType::FLOAT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 0.0f );
}

TEST_F( InterpreterCastExprTest, bool_to_char ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), BaseType::CHAR ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}

TEST_F( InterpreterCastExprTest, bool_to_bool ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), BaseType::BOOL ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, true );
}

/* void



*/

TEST_F( InterpreterCastExprTest, cast_void ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, 100 ), BaseType::VOID ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), VoidValueException );
}

/* Arrays


*/

TEST_F( InterpreterCastExprTest, int_to_int_arr ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_LITERAL( BaseType::INT, 10 ), Type::buildTypeArrayTypeFromBase( BaseType::INT ) ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}

TEST_F( InterpreterCastExprTest, int_arr_to_float ) {
  auto expr{ MAKE_CAST_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                               MAKE_LITERAL( BaseType::INT, 3 ) ),
                             BaseType::FLOAT ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}

TEST_F( InterpreterCastExprTest, int_arr_to_float_arr ) {
  // [1,2,3 ] cast_to float[] -> [1.0, 2.0, 3.0]
  auto expr{ MAKE_CAST_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                               MAKE_LITERAL( BaseType::INT, 3 ) ),
                             Type::buildTypeArrayTypeFromBase( BaseType::FLOAT ) ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1.f, 2.f, 3.f ) );
}

TEST_F( InterpreterCastExprTest, array_to_array_bad_underlying_conversion ) {
  // float[] cast_to char[]
  auto expr{
      MAKE_CAST_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::FLOAT, 1.f ), MAKE_LITERAL( BaseType::FLOAT, 2.f ) ),
                      Type::buildTypeArrayTypeFromBase( BaseType::CHAR ) ) };
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *expr ), InvalidCastException );
}