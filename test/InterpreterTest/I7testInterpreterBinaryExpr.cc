#include <gtest/gtest.h>

#include <cassert>
#include <ranges>
#include <climits>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/Interpreter.h"
#include "Lexer/Limits.hpp"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterBinaryExprTest : public InterpreterTest {};

TEST_F( InterpreterBinaryExprTest, check_init ) {
  ASSERT_TRUE( true );
}

/* Logical OR
 */

TEST_F( InterpreterBinaryExprTest, logical_or ) {
  std::vector<std::tuple<bool, bool, bool>> or_schema = {
      { false, false, false }, { false, true, true }, { true, false, true }, { true, true, true } };

  for ( const auto [l, r, exp] : or_schema ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::BOOL, l ), MAKE_LITERAL( BaseType::BOOL, r ), BinaryOperator::OR ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, exp );
  }
}

TEST_F( InterpreterBinaryExprTest, logical_or_bool_only ) {
  {  // right operand not bool
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), MAKE_LITERAL( BaseType::INT, 1 ),
                                     BinaryOperator::OR ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
  {  // left operand not bool
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::BOOL, true ),
                                     BinaryOperator::OR ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

/* Logical OR
 */

TEST_F( InterpreterBinaryExprTest, logical_and ) {
  std::vector<std::tuple<bool, bool, bool>> or_schema = {
      { false, false, false }, { false, true, false }, { true, false, false }, { true, true, true } };

  for ( const auto [l, r, exp] : or_schema ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::BOOL, l ), MAKE_LITERAL( BaseType::BOOL, r ), BinaryOperator::AND ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, exp );
  }
}

TEST_F( InterpreterBinaryExprTest, logical_and_bool_only ) {
  {  // right operand not bool
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::BOOL, true ), MAKE_LITERAL( BaseType::INT, 1 ),
                                     BinaryOperator::AND ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
  {  // left operand not bool
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::BOOL, true ),
                                     BinaryOperator::AND ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

/* Equality
 */

TEST_F( InterpreterBinaryExprTest, equals_pass_base_type ) {
  /*
  t == t
  t != t
  */
  std::vector<std::pair<BaseType, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 1 } );
  arg_pairs.push_back( { BaseType::FLOAT, 1.f } );
  arg_pairs.push_back( { BaseType::CHAR, 'a' } );
  arg_pairs.push_back( { BaseType::BOOL, true } );

  for ( const auto& [type, arg] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg.copy() ), MAKE_LITERAL( type, arg.copy() ), BinaryOperator::EQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ true } );
  }
  for ( const auto& [type, arg] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg.copy() ), MAKE_LITERAL( type, arg.copy() ), BinaryOperator::NEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ false } );
  }
}

TEST_F( InterpreterBinaryExprTest, equals_fail_base_type ) {
  /*
  t1 == t2
  t1 != t2
  */
  std::vector<std::pair<BaseType, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 1 } );
  arg_pairs.push_back( { BaseType::FLOAT, 1.f } );
  arg_pairs.push_back( { BaseType::CHAR, 'a' } );
  arg_pairs.push_back( { BaseType::BOOL, true } );

  std::vector<std::pair<BaseType, Value>> arg_pairs2;
  arg_pairs2.push_back( { BaseType::INT, 2 } );
  arg_pairs2.push_back( { BaseType::FLOAT, 2.f } );
  arg_pairs2.push_back( { BaseType::CHAR, 'b' } );
  arg_pairs2.push_back( { BaseType::BOOL, false } );

  for ( const auto& [pair1, pair2] : std::views::zip( arg_pairs, arg_pairs2 ) ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( pair1.first, pair1.second.copy() ),
                                     MAKE_LITERAL( pair2.first, pair2.second.copy() ), BinaryOperator::EQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ false } );
  }

  for ( const auto& [pair1, pair2] : std::views::zip( arg_pairs, arg_pairs2 ) ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( pair1.first, pair1.second.copy() ),
                                     MAKE_LITERAL( pair2.first, pair2.second.copy() ), BinaryOperator::NEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ true } );
  }
}

TEST_F( InterpreterBinaryExprTest, equals_pass_arr_type_single_elem ) {
  /*
  [t] == [t]
  [t] != [t]
  */
  std::vector<std::pair<BaseType, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 1 } );
  arg_pairs.push_back( { BaseType::FLOAT, 1.f } );
  arg_pairs.push_back( { BaseType::CHAR, 'a' } );
  arg_pairs.push_back( { BaseType::BOOL, true } );

  for ( const auto& [type, arg] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ), BinaryOperator::EQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ true } );
  }
  for ( const auto& [type, arg] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ), BinaryOperator::NEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ false } );
  }
}

TEST_F( InterpreterBinaryExprTest, equals_fail_arr_type_single_elem ) {
  /*
  [t1] == [t2]
  [t1] != [t2]
  */
  std::vector<std::pair<BaseType, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 1 } );
  arg_pairs.push_back( { BaseType::FLOAT, 1.f } );
  arg_pairs.push_back( { BaseType::CHAR, 'a' } );
  arg_pairs.push_back( { BaseType::BOOL, true } );

  std::vector<std::pair<BaseType, Value>> arg_pairs2;
  arg_pairs2.push_back( { BaseType::INT, 2 } );
  arg_pairs2.push_back( { BaseType::FLOAT, 2.f } );
  arg_pairs2.push_back( { BaseType::CHAR, 'b' } );
  arg_pairs2.push_back( { BaseType::BOOL, false } );

  for ( const auto& [pair1, pair2] : std::views::zip( arg_pairs, arg_pairs2 ) ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( pair1.first, pair1.second.copy() ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( pair2.first, pair2.second.copy() ) ),
                                     BinaryOperator::EQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ false } );
  }
  for ( const auto& [pair1, pair2] : std::views::zip( arg_pairs, arg_pairs2 ) ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( pair1.first, pair1.second.copy() ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( pair2.first, pair2.second.copy() ) ),
                                     BinaryOperator::NEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ true } );
  }
}

TEST_F( InterpreterBinaryExprTest, equals_pass_arr_type_many_elems ) {
  /*
  [t1, t2] == [t1, t2]
  [t1, t2] != [t1, t2]
  */
  std::vector<std::tuple<BaseType, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 1 } );
  arg_pairs.push_back( { BaseType::FLOAT, 1.f } );
  arg_pairs.push_back( { BaseType::CHAR, 'a' } );
  arg_pairs.push_back( { BaseType::BOOL, true } );

  for ( const auto& [type, arg] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ), BinaryOperator::EQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ true } );
  }
  for ( const auto& [type, arg] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( type, arg.copy() ) ), BinaryOperator::NEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ false } );
  }
}

TEST_F( InterpreterBinaryExprTest, equals_fail_arr_type_multiple_elems ) {
  /*
  [t1, t2] == [t1, t3]
  [t1, t2] != [t1, t3]
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 1, 2, 3 } );
  arg_pairs.push_back( { BaseType::FLOAT, 1.f, 2.f, 3.f } );
  arg_pairs.push_back( { BaseType::CHAR, 'a', 'b', 'c' } );
  arg_pairs.push_back( { BaseType::BOOL, true, false, true } );

  for ( const auto& [type, t1, t2, t3] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR(
        MAKE_ARR_LITERAL( MAKE_LITERAL( type, t1.copy() ), MAKE_LITERAL( type, t2.copy() ) ),
        MAKE_ARR_LITERAL( MAKE_LITERAL( type, t1.copy() ), MAKE_LITERAL( type, t3.copy() ) ), BinaryOperator::EQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ false } );
  }
  for ( const auto& [type, t1, t2, t3] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR(
        MAKE_ARR_LITERAL( MAKE_LITERAL( type, t1.copy() ), MAKE_LITERAL( type, t2.copy() ) ),
        MAKE_ARR_LITERAL( MAKE_LITERAL( type, t1.copy() ), MAKE_LITERAL( type, t3.copy() ) ), BinaryOperator::NEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ true } );
  }
}

/* Relative


*/

TEST_F( InterpreterBinaryExprTest, greater_than_int ) {
  /*
  t1 > t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 2, 1, true } );
  arg_pairs.push_back( { BaseType::INT, 2, 2, false } );
  arg_pairs.push_back( { BaseType::INT, 2, 3, false } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::GT ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, greater_or_equal_int ) {
  /*
  t1 >= t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 2, 1, true } );
  arg_pairs.push_back( { BaseType::INT, 2, 2, true } );
  arg_pairs.push_back( { BaseType::INT, 2, 3, false } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::GEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, less_than_int ) {
  /*
  t1 < t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 2, 1, false } );
  arg_pairs.push_back( { BaseType::INT, 2, 2, false } );
  arg_pairs.push_back( { BaseType::INT, 2, 3, true } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::LT ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, less_or_equal_int ) {
  /*
  t1 <= t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 2, 1, false } );
  arg_pairs.push_back( { BaseType::INT, 2, 2, true } );
  arg_pairs.push_back( { BaseType::INT, 2, 3, true } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::LEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, greater_than_float ) {
  /*
  t1 > t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 1.f, true } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 2.f, false } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 3.f, false } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::GT ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, greater_or_equal_float ) {
  /*
  t1 >= t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 1.f, true } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 2.f, true } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 3.f, false } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::GEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, less_than_float ) {
  /*
  t1 < t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 1.f, false } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 2.f, false } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 3.f, true } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::LT ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, less_or_equal_float ) {
  /*
  t1 <= t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 1.f, false } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 2.f, true } );
  arg_pairs.push_back( { BaseType::FLOAT, 2.f, 3.f, true } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::LEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, greater_than_char ) {
  /*
  t1 > t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::CHAR, 'a', 'A', true } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'A', false } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'Z', false } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::GT ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, greater_or_equal_char ) {
  /*
  t1 >= t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::CHAR, 'a', 'A', true } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'A', true } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'Z', false } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::GEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, less_than_char ) {
  /*
  t1 < t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::CHAR, 'a', 'A', false } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'A', false } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'Z', true } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::LT ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, less_or_equal_char ) {
  /*
  t1 <= t2
  */
  std::vector<std::tuple<BaseType, Value, Value, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::CHAR, 'a', 'A', false } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'A', true } );
  arg_pairs.push_back( { BaseType::CHAR, 'A', 'Z', true } );

  for ( const auto& [type, arg1, arg2, res] : arg_pairs ) {
    auto bin_expr{
        MAKE_BINARY_EXPR( MAKE_LITERAL( type, arg1.copy() ), MAKE_LITERAL( type, arg2.copy() ), BinaryOperator::LEQ ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, res );
  }
}

TEST_F( InterpreterBinaryExprTest, comparative_incompatible_types ) {
  std::vector<std::tuple<BaseType, Value, BaseType, Value>> arg_pairs;
  arg_pairs.push_back( { BaseType::INT, 2.f, BaseType::CHAR, 'c' } );
  arg_pairs.push_back( { BaseType::CHAR, 2.f, BaseType::INT, 1 } );

  for ( const auto& [type1, arg1, type2, arg2] : arg_pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( type1, arg1.copy() ), MAKE_LITERAL( type2, arg2.copy() ),
                                     BinaryOperator::GT ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

/* Numeric



*/

TEST_F( InterpreterBinaryExprTest, addition_int ) {
  std::vector<std::tuple<int, int, int>> pairs = { { 5, 10, 15 },
                                                   { -5, -10, -15 },
                                                   { 0, 0, 0 },
                                                   { 10, 0, 10 },
                                                   { INT_MAX, 1, INT_MAX },
                                                   { INT_MAX, INT_MAX, INT_MAX },
                                                   { INT_MIN, -1, INT_MIN },
                                                   { INT_MIN, INT_MIN, INT_MIN } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( arg2 ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
}

TEST_F( InterpreterBinaryExprTest, subtraction_int ) {
  std::vector<std::tuple<int, int, int>> pairs = { { 10, 5, 5 },
                                                   { -10, -5, -5 },
                                                   { 10, 0, 10 },
                                                   { 0, 10, -10 },
                                                   { 5, INT_MIN, INT_MAX },
                                                   { INT_MAX, INT_MIN, INT_MAX },
                                                   { -5, INT_MAX, INT_MIN },
                                                   { INT_MIN, INT_MAX, INT_MIN } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( arg2 ).copy() ), BinaryOperator::SUB ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
}

TEST_F( InterpreterBinaryExprTest, multiplication_int ) {
  std::vector<std::tuple<int, int, int>> pairs = { { 5, 10, 50 },
                                                   { -5, 10, -50 },
                                                   { -5, -10, 50 },
                                                   { 10, 0, 0 },
                                                   { ( INT_MAX / 2 + 1 ), 2, INT_MAX },
                                                   { INT_MAX, INT_MAX, INT_MAX },
                                                   { ( INT_MAX / 2 + 1 ), -2, INT_MIN },
                                                   { INT_MAX, INT_MIN, INT_MIN } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( arg2 ).copy() ), BinaryOperator::MUL ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
}

TEST_F( InterpreterBinaryExprTest, division_integer ) {
  std::vector<std::pair<int, int>> pairs = { { 10, 2 }, { 1, 0 }, { 0, 0 } };

  for ( const auto& [arg1, arg2] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( arg2 ).copy() ), BinaryOperator::DIV ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), InvalidOperationException );
  }
}

TEST_F( InterpreterBinaryExprTest, modulo_int ) {
  std::vector<std::tuple<int, int, int>> pairs = { { 10, 3, 1 }, { -10, 3, -1 }, { 10, -3, 1 }, { INT_MIN, -1, 0 } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( arg2 ).copy() ), BinaryOperator::MOD ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( 10 ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( 0 ).copy() ), BinaryOperator::MOD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), InvalidOperationException );
  }
}

TEST_F( InterpreterBinaryExprTest, addition_float ) {
  std::vector<std::tuple<float, float, float>> pairs = { { 5.5f, 2.2f, 7.7f },
                                                         { -5.5f, -2.2f, -7.7f },
                                                         { tkom_limits::MAX_FLOAT, 100.0f, tkom_limits::MAX_FLOAT },
                                                         { tkom_limits::MIN_FLOAT, -100.0f, tkom_limits::MIN_FLOAT } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( arg2 ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
}

TEST_F( InterpreterBinaryExprTest, subtraction_float ) {
  std::vector<std::tuple<float, float, float>> pairs = {
      { 5.5f, 2.2f, 3.3f },
      { ( tkom_limits::MAX_FLOAT / 2.0f ), tkom_limits::MIN_FLOAT, tkom_limits::MAX_FLOAT },
      { ( tkom_limits::MIN_FLOAT / 2.0f ), tkom_limits::MAX_FLOAT, tkom_limits::MIN_FLOAT } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( arg2 ).copy() ), BinaryOperator::SUB ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
}

TEST_F( InterpreterBinaryExprTest, multiplication_float ) {
  std::vector<std::tuple<float, float, float>> pairs = { { 2.5f, 4.0f, 10.0f },
                                                         { 10.5f, 0.0f, 0.0f },
                                                         { tkom_limits::MAX_FLOAT, 2.0f, tkom_limits::MAX_FLOAT },
                                                         { tkom_limits::MAX_FLOAT, -2.0f, tkom_limits::MIN_FLOAT } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( arg2 ).copy() ), BinaryOperator::MUL ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
}

TEST_F( InterpreterBinaryExprTest, division_float ) {
  std::vector<std::tuple<float, float, float>> pairs = { { 10.0f, 2.5f, 4.0f },
                                                         { tkom_limits::MAX_FLOAT, 0.5f, tkom_limits::MAX_FLOAT },
                                                         { tkom_limits::MIN_FLOAT, 0.5f, tkom_limits::MIN_FLOAT } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( arg2 ).copy() ), BinaryOperator::DIV ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( 10.0f ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( 0.0f ).copy() ), BinaryOperator::DIV ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), InvalidOperationException );
  }
}

TEST_F( InterpreterBinaryExprTest, modulo_float ) {
  std::vector<std::tuple<float, float, float>> pairs = { { 5.5f, 2.0f, 1.5f }, { -5.5f, 2.0f, -1.5f } };

  for ( const auto& [arg1, arg2, res] : pairs ) {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( arg1 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( arg2 ).copy() ), BinaryOperator::MOD ) };
    Interpreter IT{ nullptr };
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value( res ).copy() );
  }
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( 5.5f ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( 0.0f ).copy() ), BinaryOperator::MOD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), InvalidOperationException );
  }
}

TEST_F( InterpreterBinaryExprTest, FallbackTypeMismatchesThrow ) {
  // Mixed Types (Int LHS, Float RHS) [cite: 32]
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( 5 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( 5.5f ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }

  // Mixed Types (Float LHS, Int RHS) [cite: 32]
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( 5.5f ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( 5 ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

TEST_F( InterpreterBinaryExprTest, unsupported_ops ) {
  // int(5) + float(5.5)
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( 5 ).copy() ),
                                     MAKE_LITERAL( BaseType::FLOAT, Value( 5.5f ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }

  // float(5.5) + int(5)
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, Value( 5.5f ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( 5 ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

TEST_F( InterpreterBinaryExprTest, unsupported_ops_type ) {
  // string("a") + string("b")
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::CHAR, 'a' ) ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::CHAR, 'b' ) ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), InvalidOperationException );
  }

  // int(5) + string("a")
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, Value( 5 ).copy() ),
                                     MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::CHAR, 'a' ) ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }

  // bool(true) + int(1)
  {
    auto bin_expr{ MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::BOOL, Value( true ).copy() ),
                                     MAKE_LITERAL( BaseType::INT, Value( 1 ).copy() ), BinaryOperator::ADD ) };
    Interpreter IT{ nullptr };
    ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

/* CONCAT




*/

TEST_F( InterpreterBinaryExprTest, array_concatenation_different_lengths ) {
  // [1] ++ [2, 3, 4] -> [1, 2, 3, 4]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 2 ), MAKE_LITERAL( BaseType::INT, 3 ),
                                          MAKE_LITERAL( BaseType::INT, 4 ) ),
                        BinaryOperator::CONCAT );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2, 3, 4 ) );
}

TEST_F( InterpreterBinaryExprTest, array_concatenation_duplicates ) {
  // [1, 2] ++ [2, 1] -> [1, 2, 2, 1]
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 2 ), MAKE_LITERAL( BaseType::INT, 1 ) ), BinaryOperator::CONCAT );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2, 2, 1 ) );
}

TEST_F( InterpreterBinaryExprTest, array_concatenation_empty_rhs ) {
  // [1, 2] ++ [] -> [1, 2]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL(), BinaryOperator::CONCAT );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2 ) );
}

TEST_F( InterpreterBinaryExprTest, array_concatenation_empty_lhs ) {
  // [] ++ [1, 2] -> [1, 2]
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_ARR_LITERAL(), MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
      BinaryOperator::CONCAT );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2 ) );
}

TEST_F( InterpreterBinaryExprTest, array_concatenation_both_empty ) {
  // [] ++ [] -> []
  Interpreter IT{ nullptr };
  auto bin_expr = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL(), MAKE_ARR_LITERAL(), BinaryOperator::CONCAT );
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterBinaryExprTest, array_concatenation_type_incompatible ) {
  // ['a'] ++ [1] -> []
  Interpreter IT{ nullptr };
  auto bin_expr = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::CHAR, 'a' ) ),
                                    MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ) ), BinaryOperator::CONCAT );
  ASSERT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
}

TEST_F( InterpreterBinaryExprTest, array_self_concatenation ) {
  /*
  var int[] x = [1, 2]
  x ++ x -> [1, 2, 1, 2]
  */
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *MAKE_DECL_VAR( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                            MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ) ) );
  auto bin_expr = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_ID( "x" ), BinaryOperator::CONCAT );
  IT.visit( *bin_expr );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2, 1, 2 ) );
}

TEST_F( InterpreterBinaryExprTest, array_concat_primitive_rhs_exception ) {
  // [1, 2] ++ 3
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_LITERAL( BaseType::INT, 3 ), BinaryOperator::CONCAT );
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
}

TEST_F( InterpreterBinaryExprTest, array_nested_concat ) {
  /*
  [[1, 2], [3, 4]] ++ [[5, 6]] -> [[1, 2], [3, 4], [5, 6]]
  */
  Interpreter IT{ nullptr };
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 4 ) ) ),

      MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 5 ), MAKE_LITERAL( BaseType::INT, 6 ) ) ),
      BinaryOperator::CONCAT );
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false,
                   Value::makeArray( Value::makeArray( 1, 2 ), Value::makeArray( 3, 4 ), Value::makeArray( 5, 6 ) ) );
}

/* DIFF



*/

TEST_F( InterpreterBinaryExprTest, array_diff_single_removal ) {
  // [1, 2, 3] -- [2] -> [1, 3]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 3 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 2 ) ), BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 3 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_baseline_spec ) {
  // [1, 2, 2, 2, 3, 3] -- [1, 2, 3] -> [2, 2, 3]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 2 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 3 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 3 ) ),
                        BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 2, 2, 3 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_total_annihilation ) {
  // [1, 2, 2] -- [1, 2, 2] -> []
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 2 ) ),
                        BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterBinaryExprTest, array_diff_excess_rhs_multiplicity ) {
  // [1, 2] -- [2, 2, 2] -> [1]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 2 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 2 ) ),
                        BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_fifo_order_preservation ) {
  // [3, 1, 2, 1] -- [1] -> [3, 2, 1]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 1 ),
                                          MAKE_LITERAL( BaseType::INT, 2 ), MAKE_LITERAL( BaseType::INT, 1 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ) ), BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 3, 2, 1 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_no_overlap ) {
  // [1, 2] -- [3, 4] -> [1, 2]
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 4 ) ), BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_rhs_uncontained_elements ) {
  // [1, 2, 2] -- [2, 3, 4] -> [1, 2]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 2 ), MAKE_LITERAL( BaseType::INT, 3 ),
                                          MAKE_LITERAL( BaseType::INT, 4 ) ),
                        BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_interleaved_removal ) {
  // [5, 4, 5, 4, 5] -- [5, 5] -> [4, 4, 5]
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 5 ), MAKE_LITERAL( BaseType::INT, 4 ),
                        MAKE_LITERAL( BaseType::INT, 5 ), MAKE_LITERAL( BaseType::INT, 4 ),
                        MAKE_LITERAL( BaseType::INT, 5 ) ),
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 5 ), MAKE_LITERAL( BaseType::INT, 5 ) ), BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 4, 4, 5 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_primitive_lhs_exception ) {
  // 2 -- [1, 2]
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_LITERAL( BaseType::INT, 2 ),
      MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ), BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
}

TEST_F( InterpreterBinaryExprTest, array_diff_empty_lhs ) {
  // [] -- [1, 2, 3] -> []
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL(),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 3 ) ),
                        BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterBinaryExprTest, array_diff_empty_rhs ) {
  // [1, 2, 3] -- [] -> [1, 2, 3]
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                          MAKE_LITERAL( BaseType::INT, 3 ) ),
                        MAKE_ARR_LITERAL(), BinaryOperator::DIFF );
  Interpreter IT{ nullptr };
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( 1, 2, 3 ) );
}

TEST_F( InterpreterBinaryExprTest, array_diff_both_empty ) {
  // [] -- [] -> []
  Interpreter IT{ nullptr };
  auto bin_expr = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL(), MAKE_ARR_LITERAL(), BinaryOperator::DIFF );
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterBinaryExprTest, array_self_diff ) {
  /*
  var int[] x = [1, 2]
  x -- x -> []
  */
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *MAKE_DECL_VAR( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                            MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ) ) );
  auto bin_expr = MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_ID( "x" ), BinaryOperator::DIFF );
  IT.visit( *bin_expr );
  assertAccTopVal( IT, false, Value::makeArray() );
}

TEST_F( InterpreterBinaryExprTest, array_nested_diff ) {
  /*
  [[1, 2], [3, 4]] -- [[3, 4]] -> [[1, 2]]
  */
  Interpreter IT{ nullptr };
  auto bin_expr = MAKE_BINARY_EXPR(
      MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ),
                        MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 4 ) ) ),

      MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 3 ), MAKE_LITERAL( BaseType::INT, 4 ) ) ),
      BinaryOperator::DIFF );
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value::makeArray( Value::makeArray( 1, 2 ) ) );
}

/* ACCESS


*/

TEST_F( InterpreterBinaryExprTest, array_access ) {
  {  // [10, 20, 30][0] -> 10
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 10 ), MAKE_LITERAL( BaseType::INT, 20 ),
                                            MAKE_LITERAL( BaseType::INT, 30 ) ),
                          MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ 10 } );
  }
  {  // [10, 20, 30][1] -> 20
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 10 ), MAKE_LITERAL( BaseType::INT, 20 ),
                                            MAKE_LITERAL( BaseType::INT, 30 ) ),
                          MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::ACCESS );
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ 20 } );
  }
  {  // [10, 20, 30][2] -> 30
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 10 ), MAKE_LITERAL( BaseType::INT, 20 ),
                                            MAKE_LITERAL( BaseType::INT, 30 ) ),
                          MAKE_LITERAL( BaseType::INT, 2 ), BinaryOperator::ACCESS );
    IT.visit( *bin_expr );
    assertAccSize( IT, 1u );
    assertAccTopVal( IT, false, Value{ 30 } );
  }
}

TEST_F( InterpreterBinaryExprTest, array_access_nested_reads ) {
  // [[10, 20], [30, 40]][1][0] -> 30
  Interpreter IT{ nullptr };
  auto nested_expr = MAKE_BINARY_EXPR(
      MAKE_BINARY_EXPR(
          MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 10 ), MAKE_LITERAL( BaseType::INT, 20 ) ),
                            MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 30 ), MAKE_LITERAL( BaseType::INT, 40 ) ) ),
          MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::ACCESS ),
      MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  IT.visit( *nested_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value{ 30 } );
}

TEST_F( InterpreterBinaryExprTest, array_access_out_of_bounds ) {
  // [1, 2, 3][-1]
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                            MAKE_LITERAL( BaseType::INT, 3 ) ),
                          MAKE_LITERAL( BaseType::INT, -1 ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), IndexOutOfBoundsException );
  }

  // [1, 2, 3][3]
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                            MAKE_LITERAL( BaseType::INT, 3 ) ),
                          MAKE_LITERAL( BaseType::INT, 3 ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), IndexOutOfBoundsException );
  }

  // [1, 2, 3][5]
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                            MAKE_LITERAL( BaseType::INT, 3 ) ),
                          MAKE_LITERAL( BaseType::INT, 5 ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), IndexOutOfBoundsException );
  }

  // [][0]
  {
    Interpreter IT{ nullptr };
    auto bin_expr = MAKE_BINARY_EXPR( MAKE_ARR_LITERAL(), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), IndexOutOfBoundsException );
  }
}

TEST_F( InterpreterBinaryExprTest, array_access_invalid_index_type ) {
  // [1, 2, 3][1.0f]
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                            MAKE_LITERAL( BaseType::INT, 3 ) ),
                          MAKE_LITERAL( BaseType::FLOAT, 1.0f ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }

  // [1, 2, 3]['1']
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                            MAKE_LITERAL( BaseType::INT, 3 ) ),
                          MAKE_LITERAL( BaseType::CHAR, '1' ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }

  // [1, 2, 3][true]
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                            MAKE_LITERAL( BaseType::INT, 3 ) ),
                          MAKE_LITERAL( BaseType::BOOL, true ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

TEST_F( InterpreterBinaryExprTest, array_access_invalid_base_type ) {
  // 5[0]
  {
    Interpreter IT{ nullptr };
    auto bin_expr =
        MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::INT, 5 ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }

  // 5.5f[0]
  {
    Interpreter IT{ nullptr };
    auto bin_expr = MAKE_BINARY_EXPR( MAKE_LITERAL( BaseType::FLOAT, 5.5f ), MAKE_LITERAL( BaseType::INT, 0 ),
                                      BinaryOperator::ACCESS );
    EXPECT_THROW( IT.visit( *bin_expr ), NotAllowedTypeException );
  }
}

TEST_F( InterpreterBinaryExprTest, array_access_string_read )
// "abc"[0]
{
  Interpreter IT{ nullptr };
  auto bin_expr =
      MAKE_BINARY_EXPR( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::CHAR, 'a' ), MAKE_LITERAL( BaseType::CHAR, 'b' ),
                                          MAKE_LITERAL( BaseType::CHAR, 'c' ) ),
                        MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS );
  IT.visit( *bin_expr );
  assertAccSize( IT, 1u );
  assertAccTopVal( IT, false, Value{ 'a' } );
}

TEST_F( InterpreterBinaryExprTest, array_access_org_unchanged ) {
  // int[][] arr = [[10], [20]]
  // int[] x = arr[1]
  auto arr_init = MAKE_DECL_CONST( "arr", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 2u ),
                                   MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 10 ) ),
                                                     MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 20 ) ) ) );
  auto move_expr =
      MAKE_DECL_CONST( "x", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 1u ),
                       MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::ACCESS ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *arr_init );
  IT.visit( *move_expr );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "arr" ).value().get().getValue() ),
             Value::makeArray( Value::makeArray( 10 ), Value::makeArray( 20 ) ) );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), Value::makeArray( 20 ) );
}

TEST_F( InterpreterBinaryExprTest, array_access_valid_writes ) {
  // var int[] arr = [1, 2, 3]
  // arr[0] = 99 -> [99, 2, 3]
  {
    auto init = MAKE_DECL_VAR( "arr", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 1u ),
                               MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ) );
    auto write = MAKE_ASSIGNMENT_EXPR(
        MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS ),
        MAKE_LITERAL( BaseType::INT, 99 ) );

    Interpreter IT{ nullptr };
    ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
    IT.visit( *init );
    IT.visit( *write );
    ASSERT_EQ( *( ITF::env( IT ).getVarByName( "arr" ).value().get().getValue() ), Value::makeArray( 99, 2, 3 ) );
  }

  // var int[] arr = [1, 2, 3]
  // arr[2] = 99 -> Expect [1, 2, 99]
  {
    auto init = MAKE_DECL_VAR( "arr", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 1u ),
                               MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ),
                                                 MAKE_LITERAL( BaseType::INT, 3 ) ) );
    auto write = MAKE_ASSIGNMENT_EXPR(
        MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 2 ), BinaryOperator::ACCESS ),
        MAKE_LITERAL( BaseType::INT, 99 ) );

    Interpreter IT{ nullptr };
    ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
    IT.visit( *init );
    IT.visit( *write );
    ASSERT_EQ( *( ITF::env( IT ).getVarByName( "arr" ).value().get().getValue() ), Value::makeArray( 1, 2, 99 ) );
  }
}

TEST_F( InterpreterBinaryExprTest, array_access_valid_writes_nested_arr )
// var int[][] arr = [[1, 2]]
// arr[0][1] = 99 -> [[1, 99]]
{
  auto init = MAKE_DECL_VAR(
      "arr", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 2u ),
      MAKE_ARR_LITERAL( MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ) ) );

  auto write = MAKE_ASSIGNMENT_EXPR(
      MAKE_BINARY_EXPR( MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS ),
                        MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::ACCESS ),
      MAKE_LITERAL( BaseType::INT, 99 ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *init );
  IT.visit( *write );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "arr" ).value().get().getValue() ),
             Value::makeArray( Value::makeArray( 1, 99 ) ) );
}

TEST_F( InterpreterBinaryExprTest, array_access_type_inside_mismatch )
// var int[] arr = [1, 2]
// arr[0] = 'a';
{
  auto init = MAKE_DECL_VAR( "arr", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 1u ),
                             MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ) );
  auto type_mutate = MAKE_ASSIGNMENT_EXPR(
      MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS ),
      MAKE_LITERAL( BaseType::CHAR, 'a' ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *init );
  ASSERT_THROW( IT.visit( *type_mutate ), NotAllowedTypeException );
}

TEST_F( InterpreterBinaryExprTest, arr_read_and_write_to_itself )
// var int[] arr = [1,2]
// arr[0] = arr[0]
{
  auto init = MAKE_DECL_VAR( "arr", Type::buildTypeArrayFromBaseNRec( BaseType::INT, 1u ),
                             MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) ) );
  auto self_assign = MAKE_ASSIGNMENT_EXPR(
      MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS ),
      MAKE_BINARY_EXPR( MAKE_ID( "arr" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS ) );

  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *init );
  IT.visit( *self_assign );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "arr" ).value().get().getValue() ), Value::makeArray( 1, 2 ) );
}
