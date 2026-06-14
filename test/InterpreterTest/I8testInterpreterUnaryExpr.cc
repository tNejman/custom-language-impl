#include <gtest/gtest.h>

#include "Interpreter/Interpreter.h"
#include "Lexer/Limits.hpp"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterUnaryExprTest : public InterpreterTest {};

TEST_F( InterpreterUnaryExprTest, check_init ) {
  ASSERT_TRUE( true );
}

/*
NEGATION:
 - int positive -> negative
 - int negative -> positive
 - int zero -> zero
 - same for float

 - int and float MIN reversed -> cap to MAX
 - other types -> err
*/

/*
NOT
 - true for false
 - false for true

 - other types -> err
*/

/*
REV & LEN
 - list
*/

/* NEG



*/

TEST_F( InterpreterUnaryExprTest, negation_positive_int_returns_negative ) {
  // -(1) -> -1
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::INT, 1 ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, -1 );
}

TEST_F( InterpreterUnaryExprTest, negation_negative_int_returns_positive ) {
  // -(-1) -> 1
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::INT, -1 ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 1 );
}

TEST_F( InterpreterUnaryExprTest, negation_zero_int_returns_zero ) {
  // -(0) -> 0
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::INT, 0 ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 0 );
}

TEST_F( InterpreterUnaryExprTest, negation_positive_float_returns_negative ) {
  // -(12.34) -> -12.34
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, 12.34f ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, -12.34f );
}

TEST_F( InterpreterUnaryExprTest, negation_negative_float_returns_positive ) {
  // -(-12.34) -> 12.34
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, -12.34f ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 12.34f );
}

TEST_F( InterpreterUnaryExprTest, negation_zero_float_returns_negated_zero ) {
  // -(0.0) -> -0.0
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, 0.0f ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, -0.0f );
}

TEST_F( InterpreterUnaryExprTest, negation_int_min_caps_to_int_max ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::INT, tkom_limits::MIN_INT ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, tkom_limits::MAX_INT );
}

TEST_F( InterpreterUnaryExprTest, negation_float_min_caps_to_float_max ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, tkom_limits::MIN_FLOAT ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, tkom_limits::MAX_FLOAT );
}

TEST_F( InterpreterUnaryExprTest, negation_invalid_type_bool_throws_error ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), UnaryOperator::NEG ) };
  Interpreter IT{ nullptr };
  ASSERT_THROW( IT.visit( *expr ), NotAllowedTypeException );
}

/* NOT



*/

TEST_F( InterpreterUnaryExprTest, not_true_returns_false ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), UnaryOperator::NOT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, false );
}

TEST_F( InterpreterUnaryExprTest, not_false_returns_true ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::BOOL, false ), UnaryOperator::NOT ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, true );
}

TEST_F( InterpreterUnaryExprTest, not_invalid_type_int_throws_error ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::INT, 1 ), UnaryOperator::NOT ) };
  Interpreter IT{ nullptr };
  ASSERT_THROW( IT.visit( *expr ), NotAllowedTypeException );
}

/* REV



*/

TEST_F( InterpreterUnaryExprTest, rev_empty_list_returns_empty_list ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_ARR_LITERAL(), UnaryOperator::REV ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterUnaryExprTest, rev_populated_list_reverses_elements ) {
  // rev [1,2] -> [2,1]
  auto expr{ MAKE_UNARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                              UnaryOperator::REV ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 2, 1 ) );
}

TEST_F( InterpreterUnaryExprTest, rev_invalid_type_int_throws_error ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::INT, 42 ), UnaryOperator::REV ) };
  Interpreter IT{ nullptr };
  ASSERT_THROW( IT.visit( *expr ), NotAllowedTypeException );
}

/* LEN



*/

TEST_F( InterpreterUnaryExprTest, len_empty_list_returns_zero ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_ARR_LITERAL(), UnaryOperator::LEN ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 0 );
}

TEST_F( InterpreterUnaryExprTest, len_populated_list_returns_correct_length ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 10 ), MAKE_LITERAL( BaseType::INT, 20 ),
                                                MAKE_LITERAL( BaseType::INT, 30 ) ),
                              UnaryOperator::LEN ) };
  Interpreter IT{ nullptr };
  IT.visit( *expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, 3 );
}

TEST_F( InterpreterUnaryExprTest, len_invalid_type_float_throws_error ) {
  auto expr{ MAKE_UNARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, 3.14f ), UnaryOperator::LEN ) };
  Interpreter IT{ nullptr };
  ASSERT_THROW( IT.visit( *expr ), NotAllowedTypeException );
}