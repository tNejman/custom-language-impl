#include <gtest/gtest.h>

#include <ranges>
#include <variant>

#include "Exceptions/ParserExceptions/MissingExpressionException.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"
#include "TestHelperPars.hpp"

void assertProgram( const ProgramNode* program_ptr, const unsigned int expected_statement_count ) {
  ASSERT_NE( nullptr, program_ptr );
  ASSERT_EQ( expected_statement_count, program_ptr->getStatementList().size() );
}

void assertLiteralExpr( const LiteralExprNode* node, const Type& type, const Value& value ) {
  ASSERT_NE( nullptr, node );
  ASSERT_EQ( type, node->getType() );
  ASSERT_EQ( node->getValue(), value );
}

void assertArrayLiteralExprLiteralsExprOnly( const ArrayLiteralNode* node, const unsigned int expected_elem_count,
                                             const std::vector<Type>& types, const Value& values ) {
  ASSERT_TRUE( std::holds_alternative<Value::ArrayValue>( values.getData() ) );

  ASSERT_NE( nullptr, node );
  ASSERT_EQ( expected_elem_count, node->getPositions().size() );
  for ( const auto& [expr, type, val] :
        std::views::zip( node->getPositions(), types, std::get<Value::ArrayValue>( values.getData() ) ) ) {
    ASSERT_NE( nullptr, expr );
    auto* literal_ptr = dynamic_cast<const LiteralExprNode*>( expr.get() );
    ASSERT_NE( nullptr, literal_ptr );
    assertLiteralExpr( literal_ptr, type, val );
  }
}

void assertProgramCastNodeAndAssertLiteralExpr( const ProgramNode* program_ptr,
                                                const unsigned int expected_statement_count, const Type& type,
                                                const Value& value ) {
  assertProgram( program_ptr, expected_statement_count );
  auto* literal_ptr = dynamic_cast<const LiteralExprNode*>( program_ptr->getStatementList()[0].get() );
  assertLiteralExpr( literal_ptr, type, value );
}

void assertProgramCastNodeAndAssertArrayLiteralExpr( const ProgramNode* program_ptr,
                                                     const unsigned int expected_statement_count,
                                                     const unsigned int expected_elem_count,
                                                     const std::vector<Type>& types, const Value& values ) {
  assertProgram( program_ptr, expected_statement_count );
  auto* literal_ptr = dynamic_cast<const ArrayLiteralNode*>( program_ptr->getStatementList()[0].get() );
  assertArrayLiteralExprLiteralsExprOnly( literal_ptr, expected_elem_count, types, values );
}

class ParserLiteralTest : public ParserTest {};

TEST_F( ParserLiteralTest, base_type_int ) {
  auto program_ptr = initTokensAndBuildProgram( { { TokenType::INT_LITERAL, 1 }, TokenType::NEWLINE } );
  assertProgramCastNodeAndAssertLiteralExpr( program_ptr.get(), 1, Type{ BaseType::INT }, Value{ 1 } );
}

TEST_F( ParserLiteralTest, base_type_float ) {
  auto program_ptr = initTokensAndBuildProgram( { { TokenType::FLOAT_LITERAL, 1.f }, TokenType::NEWLINE } );
  assertProgramCastNodeAndAssertLiteralExpr( program_ptr.get(), 1, Type{ BaseType::FLOAT }, Value{ 1.f } );
}

TEST_F( ParserLiteralTest, base_type_char ) {
  auto program_ptr = initTokensAndBuildProgram( { { TokenType::CHAR_LITERAL, 'c' }, TokenType::NEWLINE } );
  assertProgramCastNodeAndAssertLiteralExpr( program_ptr.get(), 1, Type{ BaseType::CHAR }, Value{ 'c' } );
}

TEST_F( ParserLiteralTest, base_type_bool ) {
  auto program_ptr = initTokensAndBuildProgram( { { TokenType::BOOL_LITERAL, true }, TokenType::NEWLINE } );
  assertProgramCastNodeAndAssertLiteralExpr( program_ptr.get(), 1, Type{ BaseType::BOOL }, Value{ true } );
}

TEST_F( ParserLiteralTest, string ) {
  auto program_ptr = initTokensAndBuildProgram( { { TokenType::STRING_LITERAL, "abc" }, TokenType::NEWLINE } );
  assertProgramCastNodeAndAssertLiteralExpr( program_ptr.get(), 1, Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
                                             Value::makeArray( 'a', 'b', 'c' ) );
}

TEST_F( ParserLiteralTest, array_literal_empty ) {
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET, TokenType::RBRACKET, TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::VOID );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 0, expected_types, { Value::makeArray( 1 ) } );
}

TEST_F( ParserLiteralTest, int_array_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram(
      { TokenType::LBRACKET, { TokenType::INT_LITERAL, 1 }, TokenType::RBRACKET, TokenType::NEWLINE } );

  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::INT );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 1, expected_types, { Value::makeArray( 1 ) } );
}

TEST_F( ParserLiteralTest, float_array_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram(
      { TokenType::LBRACKET, { TokenType::FLOAT_LITERAL, 1.f }, TokenType::RBRACKET, TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::FLOAT );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 1, expected_types, Value::makeArray( 1.f ) );
}

TEST_F( ParserLiteralTest, char_array_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram(
      { TokenType::LBRACKET, { TokenType::CHAR_LITERAL, 'c' }, TokenType::RBRACKET, TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::CHAR );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 1, expected_types,
                                                  { Value::makeArray( 'c' ) } );
}

TEST_F( ParserLiteralTest, bool_array_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram(
      { TokenType::LBRACKET, { TokenType::BOOL_LITERAL, true }, TokenType::RBRACKET, TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::BOOL );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 1, expected_types,
                                                  { Value::makeArray( true ) } );
}

TEST_F( ParserLiteralTest, int_array_more_than_1_elem ) {
  // [1, 2]
  //
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET,
                                                  { TokenType::INT_LITERAL, 1 },
                                                  TokenType::COMMA,
                                                  { TokenType::INT_LITERAL, 2 },
                                                  TokenType::RBRACKET,
                                                  TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::INT );
  expected_types.push_back( BaseType::INT );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 2, expected_types, Value::makeArray( 1, 2 ) );
}

TEST_F( ParserLiteralTest, float_array_more_than_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET,
                                                  { TokenType::FLOAT_LITERAL, 1.f },
                                                  TokenType::COMMA,
                                                  { TokenType::FLOAT_LITERAL, 2.f },
                                                  TokenType::RBRACKET,
                                                  TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::FLOAT );
  expected_types.push_back( BaseType::FLOAT );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 2, expected_types,
                                                  Value::makeArray( 1.f, 2.f ) );
}

TEST_F( ParserLiteralTest, char_array_more_than_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET,
                                                  { TokenType::CHAR_LITERAL, 'c' },
                                                  TokenType::COMMA,
                                                  { TokenType::CHAR_LITERAL, 'd' },
                                                  TokenType::RBRACKET,
                                                  TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::CHAR );
  expected_types.push_back( BaseType::CHAR );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 2, expected_types,
                                                  Value::makeArray( 'c', 'd' ) );
}

TEST_F( ParserLiteralTest, bool_array_more_than_1_elem ) {
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET,
                                                  { TokenType::BOOL_LITERAL, true },
                                                  TokenType::COMMA,
                                                  { TokenType::BOOL_LITERAL, false },
                                                  TokenType::RBRACKET,
                                                  TokenType::NEWLINE } );
  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::BOOL );
  expected_types.push_back( BaseType::BOOL );
  assertProgramCastNodeAndAssertArrayLiteralExpr( program_ptr.get(), 1, 2, expected_types,
                                                  Value::makeArray( true, false ) );
}

TEST_F( ParserLiteralTest, array_literal_very_many_elements ) {
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET, { TokenType::INT_LITERAL, 1 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 2 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 3 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 4 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 5 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 6 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 7 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 8 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 9 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 10 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 11 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 12 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 13 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 14 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 15 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 16 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 17 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 18 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 19 },
                                                  TokenType::COMMA,    { TokenType::INT_LITERAL, 20 },
                                                  TokenType::RBRACKET, TokenType::NEWLINE } );

  std::vector<Type> expected_types;
  for ( int i = 0; i < 20; ++i ) {
    expected_types.push_back( BaseType::INT );
  }
  assertProgramCastNodeAndAssertArrayLiteralExpr(
      program_ptr.get(), 1, 20, expected_types,
      Value::makeArray( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 ) );
}

TEST_F( ParserLiteralTest, array_literal_nested_many_elements ) {
  // [[1, 2], [3, 4]]
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET,
                                                  TokenType::LBRACKET,
                                                  { TokenType::INT_LITERAL, 1 },
                                                  TokenType::COMMA,
                                                  { TokenType::INT_LITERAL, 2 },
                                                  TokenType::RBRACKET,
                                                  TokenType::COMMA,
                                                  TokenType::LBRACKET,
                                                  { TokenType::INT_LITERAL, 3 },
                                                  TokenType::COMMA,
                                                  { TokenType::INT_LITERAL, 4 },
                                                  TokenType::RBRACKET,
                                                  TokenType::RBRACKET,
                                                  TokenType::NEWLINE } );

  assertProgram( program_ptr.get(), 1 );
  auto literal_ptr = dynamic_cast<const ArrayLiteralNode*>( program_ptr->getStatementList()[0].get() );
  ASSERT_EQ( 2, literal_ptr->getPositions().size() );
  auto arr_pos_0 = dynamic_cast<const ArrayLiteralNode*>( literal_ptr->getPositions()[0].get() );
  auto arr_pos_1 = dynamic_cast<const ArrayLiteralNode*>( literal_ptr->getPositions()[1].get() );

  std::vector<Type> expected_types;
  expected_types.push_back( BaseType::INT );
  expected_types.push_back( BaseType::INT );
  assertArrayLiteralExprLiteralsExprOnly( arr_pos_0, 2, expected_types, Value::makeArray( 1, 2 ) );
  assertArrayLiteralExprLiteralsExprOnly( arr_pos_1, 2, expected_types, Value::makeArray( 3, 4 ) );
}

TEST_F( ParserLiteralTest, array_literal_nested_many_times ) {
  // [[[[1]]]]
  // 3 array depth
  auto program_ptr = initTokensAndBuildProgram( { TokenType::LBRACKET,
                                                  TokenType::LBRACKET,
                                                  TokenType::LBRACKET,
                                                  TokenType::LBRACKET,
                                                  { TokenType::INT_LITERAL, 1 },
                                                  TokenType::RBRACKET,
                                                  TokenType::RBRACKET,
                                                  TokenType::RBRACKET,
                                                  TokenType::RBRACKET,

                                                  TokenType::NEWLINE } );

  const IExpressionNode* underlying_literal = nullptr;
  auto* array_literal_ptr = dynamic_cast<const ArrayLiteralNode*>( program_ptr->getStatementList()[0].get() );
  for ( int i = 0; i < 3; ++i ) {
    array_literal_ptr = dynamic_cast<const ArrayLiteralNode*>( array_literal_ptr->getPositions()[0].get() );
    ASSERT_NE( nullptr, array_literal_ptr );
    ASSERT_EQ( 1, array_literal_ptr->getPositions().size() );
    ASSERT_NE( nullptr, array_literal_ptr->getPositions()[0] );

    // AstSerializerVisitor astv{};
    // array_literal_ptr->getPositions()[0]->accept( astv );
    // std::cout << astv.getString() << '\n';

    underlying_literal = array_literal_ptr->getPositions()[0].get();
  }

  auto* literal = dynamic_cast<const LiteralExprNode*>( underlying_literal );
  assertLiteralExpr( literal, Type{ BaseType::INT }, Value{ 1 } );
}

// @TODO
// TEST_F( ParserLiteralTest, array_literal_wrapped_in_parenthesis ) {
//   // ([1])
//   auto program_ptr = initTokensAndBuildProgram( { TokenType::LPAREN,
//                                                   TokenType::LBRACKET,
//                                                   { TokenType::INT_LITERAL, 1 },
//                                                   TokenType::RBRACKET,
//                                                   TokenType::RPAREN,
//                                                   TokenType::NEWLINE } );
//     assertProgramCastNodeAndAssertArrayLiteralExpr(program_ptr, 1, const unsigned int expected_elem_count, const
//     std::vector<Type> &types, const Value &values)
// }

// TEST_F( ParserLiteralTest, array_literal_expression_as_value ) {
//   // [getValue(), 1 + 2]
//   std::vector<TokenInitializer> init = {
//       TokenType::LBRACKET, { TokenType::IDENTIFIER, "getValue" }, TokenType::LPAREN,  TokenType::RPAREN,
//       TokenType::COMMA,    { TokenType::INT_LITERAL, 1 },         TokenType::OP_PLUS, { TokenType::INT_LITERAL, 2 },
//       TokenType::RBRACKET };
//   std::string res = initTokensBuildProgramAndSerialize( std::move( init ) );
//   ASSERT_EQ( "{[getValue(), {1 + 2}]}", std::move( res ) );
// }