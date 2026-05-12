#include "TestHelperLex.hpp"

class LexerPositionTest : public LexerTest {};

TEST_F( LexerPositionTest, CheckCorrectTokenPositions ) {
  assertTokensTypeAndPos( "int x = 5", { { { 1, 1 }, TokenType::T_INT },
                                         { { 1, 5 }, TokenType::IDENTIFIER, "x" },
                                         { { 1, 7 }, TokenType::OP_ASSIGN },
                                         { { 1, 9 }, TokenType::INT_LITERAL, 5 },
                                         { { 1, 10 }, TokenType::END_OF_FILE } } );
}

TEST_F( LexerPositionTest, CheckCorrectTokenPositions2 ) {
  assertTokensTypeAndPos( "var counter = 0", { { { 1, 1 }, TokenType::KW_VAR },
                                               { { 1, 5 }, TokenType::IDENTIFIER, "counter" },
                                               { { 1, 13 }, TokenType::OP_ASSIGN },
                                               { { 1, 15 }, TokenType::INT_LITERAL, 0 },
                                               { { 1, 16 }, TokenType::END_OF_FILE } } );
}
TEST_F( LexerPositionTest, CheckCorrectTokenPositions3 ) {
  assertTokensTypeAndPos( "float pi = 3.14_15_926", { { { 1, 1 }, TokenType::T_FLOAT },
                                                      { { 1, 7 }, TokenType::IDENTIFIER, "pi" },
                                                      { { 1, 10 }, TokenType::OP_ASSIGN },
                                                      { { 1, 12 }, TokenType::FLOAT_LITERAL, 3.1415926f },
                                                      { { 1, 23 }, TokenType::END_OF_FILE } } );
}

TEST_F( LexerPositionTest, CheckCorrectTokenPositions4 ) {
  assertTokensTypeAndPos( "char newline = '\\n'", { { { 1, 1 }, TokenType::T_CHAR },
                                                    { { 1, 6 }, TokenType::IDENTIFIER, "newline" },
                                                    { { 1, 14 }, TokenType::OP_ASSIGN },
                                                    { { 1, 16 }, TokenType::CHAR_LITERAL, '\n' },
                                                    { { 1, 20 }, TokenType::END_OF_FILE } } );
}
TEST_F( LexerPositionTest, CheckCorrectTokenPositions5 ) {
  assertTokensTypeAndPos( "bool is_ready = true", { { { 1, 1 }, TokenType::T_BOOL },
                                                    { { 1, 6 }, TokenType::IDENTIFIER, "is_ready" },
                                                    { { 1, 15 }, TokenType::OP_ASSIGN },
                                                    { { 1, 17 }, TokenType::BOOL_LITERAL, true },
                                                    { { 1, 21 }, TokenType::END_OF_FILE } } );
}
TEST_F( LexerPositionTest, CheckCorrectTokenPositionsMultiLine ) {
  assertTokensTypeAndPos(
      R"end(int x = 5
int y = 3)end",
      {
          { { 1, 1 }, TokenType::T_INT },
          { { 1, 5 }, TokenType::IDENTIFIER, "x" },
          { { 1, 7 }, TokenType::OP_ASSIGN },
          { { 1, 9 }, TokenType::INT_LITERAL, 5 },
          { { 1, 10 }, TokenType::NEWLINE },

          { { 2, 1 }, TokenType::T_INT },
          { { 2, 5 }, TokenType::IDENTIFIER, "y" },
          { { 2, 7 }, TokenType::OP_ASSIGN },
          { { 2, 9 }, TokenType::INT_LITERAL, 3 },
          { { 2, 10 }, TokenType::END_OF_FILE },
      } );
}

TEST_F( LexerPositionTest, UnchangingEndOfFileLocation ) {
  std::stringstream ss{ "int x = 5" };
  Lexer lexer{ ss };
  Position pos_stable{ 1, 10 };
  for ( int i = 0; i < 10; ++i ) {
    Token t = lexer.getNextToken();
    if ( i > 3 ) {
      ASSERT_EQ( TokenType::END_OF_FILE, t.type_ );
      ASSERT_EQ( pos_stable, t.position_ );
    }
  }
}

TEST_F( LexerPositionTest, EOFImmediatelyAfterNewline ) {
  assertTokensTypeAndPos(
      R"(
)",
      { { { 1, 1 }, TokenType::NEWLINE }, { { 2, 1 }, TokenType::END_OF_FILE } } );
}

TEST_F( LexerPositionTest, MultipleNewlines ) {
  assertTokensTypeAndPos(
      R"(2 + 3



)",
      { { { 1, 1 }, TokenType::INT_LITERAL, 2 },
        { { 1, 3 }, TokenType::OP_PLUS },
        { { 1, 5 }, TokenType::INT_LITERAL, 3 },
        { { 1, 6 }, TokenType::NEWLINE },
        { { 2, 1 }, TokenType::NEWLINE },
        { { 3, 1 }, TokenType::NEWLINE },
        { { 4, 1 }, TokenType::NEWLINE },
        { { 5, 1 }, TokenType::END_OF_FILE } } );
}

TEST_F( LexerPositionTest, SkippingWhitespaces ) {
  assertTokensTypeAndPos( "\t\t\t\t  int             x       =   5", { { { 1, 7 }, TokenType::T_INT },
                                                                       { { 1, 23 }, TokenType::IDENTIFIER, "x" },
                                                                       { { 1, 31 }, TokenType::OP_ASSIGN },
                                                                       { { 1, 35 }, TokenType::INT_LITERAL, 5 },
                                                                       { { 1, 36 }, TokenType::END_OF_FILE } } );
}

TEST_F( LexerPositionTest, LFendlines ) {
  assertTokensTypeAndPos(
      "\n\n",
      { { { 1, 1 }, TokenType::NEWLINE }, { { 2, 1 }, TokenType::NEWLINE }, { { 3, 1 }, TokenType::END_OF_FILE } } );
}
TEST_F( LexerPositionTest, CRLFendlines ) {
  assertTokensTypeAndPos(
      "\r\n\r\n",
      { { { 1, 1 }, TokenType::NEWLINE }, { { 2, 1 }, TokenType::NEWLINE }, { { 3, 1 }, TokenType::END_OF_FILE } } );
}