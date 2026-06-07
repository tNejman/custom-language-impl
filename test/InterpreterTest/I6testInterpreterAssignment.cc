#include <gtest/gtest.h>

#include <cassert>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "TestHelperInterpreter.hpp"

class InterpreterAssignmentTest : public InterpreterTest {};

TEST_F( InterpreterAssignmentTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( InterpreterAssignmentTest, change_val ) {
  /*
  var int x = 1
  x = 2
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto assignment = MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  IT.visit( *assignment );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), 2 );
}

TEST_F( InterpreterAssignmentTest, const_err ) {
  /*
  int x = 1
  x = 2
  */
  auto decl = MAKE_CONST_INT( "x", 1 );
  auto assignment = MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  ASSERT_THROW( IT.visit( *assignment ), InvalidAccessException );
}

TEST_F( InterpreterAssignmentTest, wrong_type ) {
  /*
  var int x = 1
  x = 'c'
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto assignment = MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::CHAR, 'c' ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  ASSERT_THROW( IT.visit( *assignment ), NotAllowedTypeException );
}

TEST_F( InterpreterAssignmentTest, new_val_using_old_val ) {
  /*
  var int x = 1
  x = x + 1
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto assignment = MAKE_ASSIGNMENT_EXPR(
      MAKE_ID( "x" ), MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 1 ), BinaryOperator::ADD ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  IT.visit( *assignment );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), 2 );
}

TEST_F( InterpreterAssignmentTest, assignment_returns_void_and_cannot_assign_void ) {
  /*
  var int x = 1
  x = x = 2 <- err
  */
  auto decl = MAKE_VAR_INT( "x", 1 );
  auto assignment =
      MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_ASSIGNMENT_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ) ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  ASSERT_THROW( IT.visit( *assignment ), VoidValueException );
}

TEST_F( InterpreterAssignmentTest, assign_to_arr_pos ) {
  /*
  var int[] x = [1]
  x[0] = 2
  */
  auto decl = MAKE_DECL_VAR( "x", Type::buildTypeArrayTypeFromBase( BaseType::INT ),
                             MAKE_ARR_LITERAL( MAKE_LITERAL( BaseType::INT, 1 ) ) );
  auto assignment = MAKE_ASSIGNMENT_EXPR(
      MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 0 ), BinaryOperator::ACCESS ),
      MAKE_LITERAL( BaseType::INT, 2 ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  IT.visit( *assignment );
  ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), Value::makeArray( 2 ) );
}

TEST_F( InterpreterAssignmentTest, assign_to_rvalue ) {
  /*
  1 = 2
  */
  auto assignment = MAKE_ASSIGNMENT_EXPR( MAKE_LITERAL( BaseType::INT, 1 ), MAKE_LITERAL( BaseType::INT, 2 ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_THROW( IT.visit( *assignment ), InvalidAccessException );
}

TEST_F( InterpreterAssignmentTest, assign_to_rvalue_expr ) {
  /*
  int x = 1
  int y = 2
  (x * y) = 2
  */
  auto decl_x = MAKE_CONST_INT( "x", 1 );
  auto decl_y = MAKE_VAR_INT( "y", 2 );
  auto assignment = MAKE_ASSIGNMENT_EXPR( MAKE_BINARY_EXPR( MAKE_ID( "x" ), MAKE_ID( "y" ), BinaryOperator::MUL ),
                                          MAKE_LITERAL( BaseType::INT, 2 ) );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl_x );
  IT.visit( *decl_y );
  ASSERT_THROW( IT.visit( *assignment ), InvalidAccessException );
}

TEST_F( InterpreterAssignmentTest, assign_to_void ) {
  /*
  def void foo() do
  done
  foo() = 1
  */
  auto func_def =
      std::make_unique<FunctionDefNode>( Position{ 1, 1 }, "foo", BaseType::VOID, makeParams(), makeBlock() );
  auto assignment =
      MAKE_ASSIGNMENT_EXPR( std::make_unique<FunctionCallNode>( Position{ 1, 1 }, "foo", makeExpressions() ),
                            MAKE_LITERAL( BaseType::INT, 1 ) );
  Interpreter IT{ nullptr };
  ITF::funcs( IT ).push_back( *func_def );
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  ASSERT_THROW( IT.visit( *assignment ), VoidValueException );
}

TEST_F( InterpreterAssignmentTest, special_assignment_int ) {
  /*
  var int x = 10
  x += 2 | x -= 2 | x *= 2 | x %= 2
  */
  std::vector<std::pair<AssignmentType, int>> assignment_types = { { AssignmentType::ADD_ASSIGN, 12 },
                                                                   { AssignmentType::SUB_ASSIGN, 8 },
                                                                   { AssignmentType::MUL_ASSIGN, 20 },
                                                                   { AssignmentType::MOD_ASSIGN, 0 } };

  for ( const auto [at, val] : assignment_types ) {
    auto decl = MAKE_VAR_INT( "x", 10 );
    auto assignment = MAKE_ASSIGNMENT_EXPR_WITH_OP( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ), at );
    Interpreter IT{ nullptr };
    ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
    IT.visit( *decl );
    IT.visit( *assignment );
    ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), val );
  }
}
TEST_F( InterpreterAssignmentTest, special_assignment_int_div_fail ) {
  // no div for ints
  auto decl = MAKE_VAR_INT( "x", 10 );
  auto assignment =
      MAKE_ASSIGNMENT_EXPR_WITH_OP( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::INT, 2 ), AssignmentType::DIV_ASSIGN );
  Interpreter IT{ nullptr };
  ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
  IT.visit( *decl );
  ASSERT_THROW( IT.visit( *assignment ), InvalidOperationException );
}

TEST_F( InterpreterAssignmentTest, special_assignment_float ) {
  /*
  var float x = 10.0
  x += 2.0 | x -= 2.0 | x *= 2.0 | x /= 2.0 | x %= 2.0
  */
  std::vector<std::pair<AssignmentType, float>> assignment_types = { { AssignmentType::ADD_ASSIGN, 12.f },
                                                                     { AssignmentType::SUB_ASSIGN, 8.f },
                                                                     { AssignmentType::MUL_ASSIGN, 20.f },
                                                                     { AssignmentType::DIV_ASSIGN, 5.f },
                                                                     { AssignmentType::MOD_ASSIGN, 0.f } };

  for ( const auto [at, val] : assignment_types ) {
    auto decl = MAKE_DECL_VAR( "x", BaseType::FLOAT, MAKE_LITERAL( BaseType::FLOAT, 10.f ) );
    auto assignment = MAKE_ASSIGNMENT_EXPR_WITH_OP( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::FLOAT, 2.f ), at );
    Interpreter IT{ nullptr };
    ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
    IT.visit( *decl );
    IT.visit( *assignment );
    ASSERT_EQ( *( ITF::env( IT ).getVarByName( "x" ).value().get().getValue() ), val );
  }
}

TEST_F( InterpreterAssignmentTest, special_assignment_char ) {
  /*
  var char x = 'c'
  x += 'b' | x -= 'b' | x *= 'b' | x /= 'b' | x %= 'b'
  */
  std::vector<AssignmentType> assignment_types = { AssignmentType::ADD_ASSIGN, AssignmentType::SUB_ASSIGN,
                                                   AssignmentType::MUL_ASSIGN, AssignmentType::DIV_ASSIGN,
                                                   AssignmentType::MOD_ASSIGN };

  for ( const auto at : assignment_types ) {
    auto decl = MAKE_DECL_VAR( "x", BaseType::CHAR, MAKE_LITERAL( BaseType::CHAR, 'c' ) );
    auto assignment = MAKE_ASSIGNMENT_EXPR_WITH_OP( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::CHAR, 'b' ), at );
    Interpreter IT{ nullptr };
    ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
    IT.visit( *decl );
    ASSERT_THROW( IT.visit( *assignment ), InvalidOperationException );
  }
}

TEST_F( InterpreterAssignmentTest, special_assignment_bool ) {
  /*
  var bool x = true
  x += true | x -= true | x *= true | x /= true | x %= true
  */
  std::vector<AssignmentType> assignment_types = { AssignmentType::ADD_ASSIGN, AssignmentType::SUB_ASSIGN,
                                                   AssignmentType::MUL_ASSIGN, AssignmentType::DIV_ASSIGN,
                                                   AssignmentType::MOD_ASSIGN };

  for ( const auto at : assignment_types ) {
    auto decl = MAKE_DECL_VAR( "x", BaseType::BOOL, MAKE_LITERAL( BaseType::BOOL, true ) );
    auto assignment = MAKE_ASSIGNMENT_EXPR_WITH_OP( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::BOOL, true ), at );
    Interpreter IT{ nullptr };
    ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
    IT.visit( *decl );
    ASSERT_THROW( IT.visit( *assignment ), InvalidOperationException );
  }
}

// TEST_F( InterpreterAssignmentTest, special_assignment_arr ) {
//   /*
//   var T[] x = [t]
//   x += [t] | x -= [t] | x *= [t] | x /= [t] | x %= [t]
//   */
//   std::vector<AssignmentType> assignment_types = { AssignmentType::ADD_ASSIGN, AssignmentType::SUB_ASSIGN,
//                                                    AssignmentType::MUL_ASSIGN, AssignmentType::DIV_ASSIGN,
//                                                    AssignmentType::MOD_ASSIGN };
//
//   for ( const auto at : assignment_types ) {
//     auto decl = MAKE_DECL_VAR( "x", BaseType::BOOL, MAKE_LITERAL( BaseType::BOOL, true ) );
//     auto assignment = MAKE_ASSIGNMENT_EXPR_WITH_OP( MAKE_ID( "x" ), MAKE_LITERAL( BaseType::BOOL, true ), at );
//     Interpreter IT{ nullptr };
//     ITF::addMockCallContext( IT, CallContext::ContextType::TOP_LEVEL, 10u );
//     IT.visit( *decl );
//     ASSERT_THROW( IT.visit( *assignment ), NotAllowedTypeException );
//   }
// }

// }