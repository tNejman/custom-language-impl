#include <gtest/gtest.h>

#include <sstream>

#include "Exceptions/LexerExceptions/FloatLiteralOutOfBoundsException.hpp"
#include "Exceptions/LexerExceptions/IntLiteralOutOfBoundsException.hpp"
#include "Exceptions/LexerExceptions/InvalidCharLiteralException.hpp"
#include "Exceptions/LexerExceptions/MalformedNumericLiteralException.hpp"
#include "Exceptions/LexerExceptions/TooLongStringLiteralException.hpp"
#include "Exceptions/LexerExceptions/UnknownEscapedCharacterException.hpp"
#include "Exceptions/LexerExceptions/UnknownSymbolException.hpp"
#include "Exceptions/LexerExceptions/UnterminatedCharLiteralException.hpp"
#include "Exceptions/LexerExceptions/UnterminatedStringLiteralException.hpp"
#include "TestHelper.hpp"

class LexerLiteralTest : public LexerTest {};

TEST_F( LexerLiteralTest, IntLiteralWithUnderscore ) {
  std::stringstream ss{ "1_000" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::INT_LITERAL, t.type_ );
  ASSERT_EQ( 1000, std::get<int>( t.value_ ) );
}

TEST_F( LexerLiteralTest, IntLiteralWithManyUnderscores ) {
  std::stringstream ss{ "1_0__00___000" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::INT_LITERAL, t.type_ );
  ASSERT_EQ( 1000000, std::get<int>( t.value_ ) );
}

TEST_F( LexerLiteralTest, IntLiteralBeginingWithUnderscore ) {
  std::stringstream ss{ "_1000" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), UnknownSymbolException );
}

TEST_F( LexerLiteralTest, IntLiteralEndingWithUnderscore ) {
  std::stringstream ss{ "1000_" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::INT_LITERAL, t.type_ );
  ASSERT_EQ( 1000, std::get<int>( t.value_ ) );
}

TEST_F( LexerLiteralTest, IntLiteralBeginingWithZeros ) {
  std::stringstream ss{ "00001" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::INT_LITERAL, t.type_ );
  ASSERT_EQ( 1, std::get<int>( t.value_ ) );
}

TEST_F( LexerLiteralTest, IntLiteralOnlyZeroes ) {
  std::stringstream ss{ "0_00_000" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::INT_LITERAL, t.type_ );
  ASSERT_EQ( 0, std::get<int>( t.value_ ) );
}

TEST_F( LexerLiteralTest, IntLiteralOutOfBounds ) {
  std::stringstream ss{ "1_000_000_000_000_000" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), IntLiteralOutOfBoundsException );
}

TEST_F( LexerLiteralTest, FloatLiteral ) {
  std::stringstream ss{ "12.34" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::FLOAT_LITERAL, t.type_ );
  ASSERT_TRUE( areFloatsKindaEqual( 12.34, std::get<float>( t.value_ ) ) );
}

TEST_F( LexerLiteralTest, FloatLiteralHexLikeIncorrect ) {
  std::stringstream ss{ "0xGHOST" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), MalformedNumericLiteralException );
}

TEST_F( LexerLiteralTest, FloatLiteralDoubleDecimalPoint ) {
  std::stringstream ss{ "12.34.56" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), MalformedNumericLiteralException );
}

TEST_F( LexerLiteralTest, FloatLiteralOutOfBounds ) {
  std::stringstream ss{
      "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), FloatLiteralOutOfBoundsException );
}

TEST_F( LexerLiteralTest, NumericLiteralLikeIdentifier ) {
  std::stringstream ss{ "123_bad_indent" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), MalformedNumericLiteralException );
}

TEST_F( LexerTest, EmptyCharLiteral ) {
  std::stringstream ss{ "''" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), InvalidCharLiteralException );
}

TEST_F( LexerLiteralTest, UnterminatedCharLiteral ) {
  std::stringstream ss{ "\'abc\'" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), UnterminatedCharLiteralException );
}

TEST_F( LexerLiteralTest, UnknownEscapedCharacterException ) {
  std::stringstream ss{ "\'\\w\'" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), UnknownEscapedCharacterException );
}

TEST_F( LexerLiteralTest, UnterminatedCharLiteral2 ) {
  std::stringstream ss{ "\'a" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), UnterminatedCharLiteralException );
}

TEST_F( LexerLiteralTest, UnterminatedCharLiteral3 ) {
  std::stringstream ss{ R"([char] v = ['a] ++ ['b'])" };
  Lexer lexer{ ss };
  for ( int i = 0; i < 6; ++i ) lexer.getNextToken();
  ASSERT_THROW( lexer.getNextToken(), UnterminatedCharLiteralException );
}

TEST_F( LexerLiteralTest, EscapedCharacters ) {
  std::stringstream ss{ R"("\n\t\\\"\'")" };
  Lexer lexer{ ss };
  const std::string comp = "\n\t\\\"\'";
  ASSERT_EQ( comp, std::get<std::string>( lexer.getNextToken().value_ ) );
}

TEST_F( LexerLiteralTest, EmptyStringLiteral ) {
  std::stringstream ss{ R"("")" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  ASSERT_EQ( TokenType::STRING_LITERAL, t.type_ );
  ASSERT_EQ( "", std::get<std::string>( t.value_ ) );
  ASSERT_EQ( TokenType::END_OF_FILE, lexer.getNextToken().type_ );
}

TEST_F( LexerLiteralTest, EsapcedBackslashAtTheEndOfStringLiteral ) {
  assertTokensType( "\"a\\\\\"", { TokenType::STRING_LITERAL, TokenType::END_OF_FILE } );
}

TEST_F( LexerLiteralTest, UnterminatedStringLiteral ) {
  std::stringstream ss( R"("abc)" );
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), UnterminatedStringLiteralException );
}

TEST_F( LexerLiteralTest, TooLongStringLiteral ) {
  std::stringstream ss{
      "\"Initial_Stream\tBackslash_Test:\\\\_Quote_Test:\\\'_Hex_Test:_Octal:_Long_UTF8_Sequence:🚀_"
      "Padding_For_Length_Boundary_Check_0123456789_ABCDE_FGHIJ_KLMNO_PQRST_UVWXY_Z_EndOfLiteral_012345678\"" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken(), TooLongStringLiteralException );
}

TEST_F( LexerLiteralTest, CompilcatedStringLiteral ) {
  std::stringstream ss{
      R"("Initial_Stream\tBackslash_Test:\\_Quote_Test:\'_Hex_Test:_Octal:_Long_UTF8_Sequence:🚀_")" };
  Lexer lexer{ ss };
  Token t = lexer.getNextToken();
  const std::string comp = "Initial_Stream\tBackslash_Test:\\_Quote_Test:\'_Hex_Test:_Octal:_Long_UTF8_Sequence:🚀_";
  ASSERT_EQ( comp, std::get<std::string>( t.value_ ) );
}