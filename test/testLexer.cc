#include <gtest/gtest.h>

#include <iostream>
#include <vector>


#include "Exceptions/LexerExceptions/UnknownSymbolException.hpp"
#include "InvalidCommentStyleException.hpp"
#include "TestHelper.hpp"

// int main( int argc, char **argv ) {
//   ::testing::InitGoogleTest( &argc, argv );
//   return RUN_ALL_TESTS();
// }

TEST_F( LexerTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( LexerTest, check_exception ) {
  ASSERT_ANY_THROW( []() {
    std::cout << "DEBUG: lambda\n";
    throw std::runtime_error( "test" );
  }() );
}

TEST_F( LexerTest, OperatorsAndSymbols ) {
  assertTokensType( "+", { TokenType::OP_PLUS } );
  assertTokensType( "++", { TokenType::OP_CONCAT } );
  assertTokensType( "+=", { TokenType::OP_ADD_ASSIGN } );

  assertTokensType( "-", { TokenType::OP_MINUS } );
  assertTokensType( "--", { TokenType::OP_DIFF } );
  assertTokensType( "-=", { TokenType::OP_SUB_ASSIGN } );

  assertTokensType( "*", { TokenType::OP_MUL } );
  assertTokensType( "*=", { TokenType::OP_MUL_ASSIGN } );

  assertTokensType( "/", { TokenType::OP_DIV } );
  assertTokensType( "/=", { TokenType::OP_DIV_ASSIGN } );

  assertTokensType( "%", { TokenType::OP_MOD } );
  assertTokensType( "%=", { TokenType::OP_MOD_ASSIGN } );

  assertTokensType( "=", { TokenType::OP_ASSIGN } );
  assertTokensType( "==", { TokenType::OP_EQ } );
  assertTokensType( "!=", { TokenType::OP_NEQ } );

  assertTokensType( ">", { TokenType::OP_GT } );
  assertTokensType( ">=", { TokenType::OP_GEQ } );
  assertTokensType( "<", { TokenType::OP_LT } );
  assertTokensType( "<=", { TokenType::OP_LEQ } );

  assertTokensType( "(", { TokenType::LPAREN } );
  assertTokensType( ")", { TokenType::RPAREN } );
  assertTokensType( "[", { TokenType::LBRACKET } );
  assertTokensType( "]", { TokenType::RBRACKET } );

  assertTokensType( "?", { TokenType::OP_FILTER } );
  assertTokensType( "->", { TokenType::OP_MAP } );
  assertTokensType( "@", { TokenType::OP_REV } );
  assertTokensType( "$", { TokenType::OP_LEN } );

  assertTokensType( ",", { TokenType::COMMA } );
  assertTokensType( ":", { TokenType::COLON } );
  assertTokensType( "\n", { TokenType::NEWLINE } );
}

TEST_F( LexerTest, FunctionDefinition ) {
  assertTokensType(
      R"end(def int calculate(int x, int y) do
    ret x + y
done)end",
      { TokenType::KW_DEF, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::LPAREN, TokenType::T_INT,
        TokenType::IDENTIFIER, TokenType::COMMA, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::RPAREN,
        TokenType::KW_DO, TokenType::NEWLINE,

        TokenType::KW_RET, TokenType::IDENTIFIER, TokenType::OP_PLUS, TokenType::IDENTIFIER, TokenType::NEWLINE,

        TokenType::KW_DONE, TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, ControlFlow ) {
  assertTokensType(
      R"end(if (a >= 10 and b <= 20) do
    a += 1
done elseif (a != b or not c) do
    b -= 1
done else do
    a = 2
    b /= 2
done)end",
      { TokenType::KW_IF,       TokenType::LPAREN,        TokenType::IDENTIFIER,  TokenType::OP_GEQ,
        TokenType::INT_LITERAL, TokenType::OP_AND,        TokenType::IDENTIFIER,  TokenType::OP_LEQ,
        TokenType::INT_LITERAL, TokenType::RPAREN,        TokenType::KW_DO,       TokenType::NEWLINE,

        TokenType::IDENTIFIER,  TokenType::OP_ADD_ASSIGN, TokenType::INT_LITERAL, TokenType::NEWLINE,

        TokenType::KW_DONE,     TokenType::KW_ELSEIF,     TokenType::LPAREN,      TokenType::IDENTIFIER,
        TokenType::OP_NEQ,      TokenType::IDENTIFIER,    TokenType::OP_OR,       TokenType::OP_NOT,
        TokenType::IDENTIFIER,  TokenType::RPAREN,        TokenType::KW_DO,       TokenType::NEWLINE,

        TokenType::IDENTIFIER,  TokenType::OP_SUB_ASSIGN, TokenType::INT_LITERAL, TokenType::NEWLINE,
        TokenType::KW_DONE,     TokenType::KW_ELSE,       TokenType::KW_DO,       TokenType::NEWLINE,
        TokenType::IDENTIFIER,  TokenType::OP_ASSIGN,     TokenType::INT_LITERAL, TokenType::NEWLINE,
        TokenType::IDENTIFIER,  TokenType::OP_DIV_ASSIGN, TokenType::INT_LITERAL, TokenType::NEWLINE,
        TokenType::KW_DONE,     TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, ConcatenationInitialization ) {
  assertTokensType( "var [int] arr = [1,2,3] ++ [4,5]",
                    { TokenType::KW_VAR,     TokenType::LBRACKET,    TokenType::T_INT,    TokenType::RBRACKET,
                      TokenType::IDENTIFIER, TokenType::OP_ASSIGN,   TokenType::LBRACKET, TokenType::INT_LITERAL,
                      TokenType::COMMA,      TokenType::INT_LITERAL, TokenType::COMMA,    TokenType::INT_LITERAL,
                      TokenType::RBRACKET,   TokenType::OP_CONCAT,   TokenType::LBRACKET, TokenType::INT_LITERAL,
                      TokenType::COMMA,      TokenType::INT_LITERAL, TokenType::RBRACKET, TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, OperatorDiff ) {
  assertTokensType( "arr = arr -- [2]", {
                                            TokenType::IDENTIFIER,
                                            TokenType::OP_ASSIGN,
                                            TokenType::IDENTIFIER,
                                            TokenType::OP_DIFF,
                                            TokenType::LBRACKET,
                                            TokenType::INT_LITERAL,
                                            TokenType::RBRACKET,
                                        } );
}

TEST_F( LexerTest, OperatorLength ) {
  assertTokensType( "var int size = $arr",
                    { TokenType::KW_VAR, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::OP_ASSIGN,
                      TokenType::OP_LEN, TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, OperatorFilter ) {
  assertTokensType( "var [int] filtered = arr ? isBig",
                    { TokenType::KW_VAR, TokenType::LBRACKET, TokenType::T_INT, TokenType::RBRACKET,
                      TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::IDENTIFIER, TokenType::OP_FILTER,
                      TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, OperatorReverse ) {
  assertTokensType( "[int] reversed = @arr",
                    { TokenType::LBRACKET, TokenType::T_INT, TokenType::RBRACKET, TokenType::IDENTIFIER,
                      TokenType::OP_ASSIGN, TokenType::OP_REV, TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, WhileBreakContinue ) {
  assertTokensType(
      R"end(while (i > 0) do
  if (i == 10) do
    break
  done elseif (i == 17) do
    continue
  done
done)end",
      { TokenType::KW_WHILE,    TokenType::LPAREN,      TokenType::IDENTIFIER, TokenType::OP_GT,
        TokenType::INT_LITERAL, TokenType::RPAREN,      TokenType::KW_DO,      TokenType::NEWLINE,

        TokenType::KW_IF,       TokenType::LPAREN,      TokenType::IDENTIFIER, TokenType::OP_EQ,
        TokenType::INT_LITERAL, TokenType::RPAREN,      TokenType::KW_DO,      TokenType::NEWLINE,

        TokenType::KW_BREAK,    TokenType::NEWLINE,

        TokenType::KW_DONE,     TokenType::KW_ELSEIF,   TokenType::LPAREN,     TokenType::IDENTIFIER,
        TokenType::OP_EQ,       TokenType::INT_LITERAL, TokenType::RPAREN,     TokenType::KW_DO,
        TokenType::NEWLINE,

        TokenType::KW_CONTINUE, TokenType::NEWLINE,

        TokenType::KW_DONE,     TokenType::NEWLINE,

        TokenType::KW_DONE,     TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, FullLineComments ) {
  assertTokensType(
      R"(#this is a full line comment
#int x = 5
#previous line was also a comment)",
      { TokenType::COMMENT, TokenType::NEWLINE, TokenType::COMMENT, TokenType::NEWLINE, TokenType::COMMENT,
        TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, CommentAfterCode ) {
  assertTokensType( "int x = 10 # comment", { TokenType::T_INT, TokenType::IDENTIFIER, TokenType::OP_ASSIGN,
                                              TokenType::INT_LITERAL, TokenType::COMMENT, TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, Whitespaces ) {
  assertTokensType( "            ", { TokenType::END_OF_FILE } );
}

TEST_F( LexerTest, OperatorChains ) {
  assertTokensType( "+=-==/=++", { TokenType::OP_ADD_ASSIGN, TokenType::OP_SUB_ASSIGN, TokenType::OP_ASSIGN,
                                   TokenType::OP_DIV_ASSIGN, TokenType::OP_CONCAT } );
  assertTokensType( "!===", { TokenType::OP_NEQ, TokenType::OP_EQ } );
  assertTokensType( "===>", { TokenType::OP_EQ, TokenType::OP_ASSIGN, TokenType::OP_GT } );
  assertTokensType( "->->", { TokenType::OP_MAP, TokenType::OP_MAP } );
  assertTokensType( "$?@%+++---",
                    { TokenType::OP_LEN, TokenType::OP_FILTER, TokenType::OP_REV, TokenType::OP_MOD,
                      TokenType::OP_CONCAT, TokenType::OP_PLUS, TokenType::OP_DIFF, TokenType::OP_MINUS } );
}

TEST_F( LexerTest, UnknownSymbols ) {
  std::stringstream ss{ "&|^~`" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken();, UnknownSymbolException );
  ASSERT_THROW( lexer.getNextToken();, UnknownSymbolException );
  ASSERT_THROW( lexer.getNextToken();, UnknownSymbolException );
  ASSERT_THROW( lexer.getNextToken();, UnknownSymbolException );
  ASSERT_THROW( lexer.getNextToken();, UnknownSymbolException );
}

TEST_F( LexerTest, InvalidCommentStyle ) {
  std::stringstream ss{ "/* block comment? */" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken();, InvalidCommentStyleException );
}

// TEST_F( LexerTest, MajorCodeChunk ) {
// vector<int> v{1, 2, 5, 7, 9, 11, 13};

// x := 5;
// l := 0;
// r := $(v)-1;
// while(l < r){
//   mid := (l+r)/2;
//   if(v[mid] <= x){
//     l = mid;
//   }else{
//     r = mid-1;
//   }
// }
// cout << r;

//   assertTokensType(
//       R"end([int] v = [1, 2, 5]
// int x = 5
// var int l = 0
// var int r = $v - 1
// while (l < r) do
//   int mid = (l + r) / 2
//   if (v[mid] <= x) do
//     l = mid
//   done else do
//     r = mid - 1
//   done
// done
// write(r)
// write('\n'))end",
//       { TokenType::LBRACKET,    TokenType::T_INT,      TokenType::RBRACKET,     TokenType::IDENTIFIER,
//         TokenType::OP_ASSIGN,   TokenType::LBRACKET,   TokenType::INT_LITERAL,  TokenType::COMMA,
//         TokenType::INT_LITERAL, TokenType::COMMA,      TokenType::INT_LITERAL,  TokenType::RBRACKET,
//         TokenType::NEWLINE,

//         TokenType::T_INT,       TokenType::IDENTIFIER, TokenType::OP_ASSIGN,    TokenType::INT_LITERAL,
//         TokenType::NEWLINE,

//         TokenType::KW_VAR,      TokenType::T_INT,      TokenType::IDENTIFIER,   TokenType::OP_ASSIGN,
//         TokenType::INT_LITERAL, TokenType::NEWLINE,

//         TokenType::KW_VAR,      TokenType::T_INT,      TokenType::IDENTIFIER,   TokenType::OP_ASSIGN,
//         TokenType::OP_LEN,      TokenType::IDENTIFIER, TokenType::OP_MINUS,     TokenType::INT_LITERAL,
//         TokenType::NEWLINE,

//         TokenType::KW_WHILE,    TokenType::LPAREN,     TokenType::IDENTIFIER,   TokenType::OP_LEQ,
//         TokenType::IDENTIFIER,  TokenType::RPAREN,     TokenType::KW_DO,        TokenType::NEWLINE,

//         TokenType::T_INT,       TokenType::IDENTIFIER, TokenType::LPAREN,       TokenType::IDENTIFIER,
//         TokenType::OP_PLUS,     TokenType::IDENTIFIER, TokenType::RPAREN,       TokenType::OP_DIV,
//         TokenType::INT_LITERAL, TokenType::NEWLINE,

//         TokenType::KW_IF,       TokenType::LPAREN,     TokenType::IDENTIFIER,   TokenType::LBRACKET,
//         TokenType::IDENTIFIER,  TokenType::RBRACKET,   TokenType::OP_LEQ,       TokenType::IDENTIFIER,
//         TokenType::RPAREN,      TokenType::KW_DO,      TokenType::NEWLINE,

//         TokenType::IDENTIFIER,  TokenType::OP_ASSIGN,  TokenType::IDENTIFIER,   TokenType::NEWLINE,

//         TokenType::KW_DONE,     TokenType::KW_ELSE,    TokenType::KW_DO,        TokenType::NEWLINE,

//         TokenType::IDENTIFIER,  TokenType::OP_ASSIGN,  TokenType::IDENTIFIER,   TokenType::OP_MINUS,
//         TokenType::INT_LITERAL, TokenType::NEWLINE,

//         TokenType::KW_DONE,     TokenType::NEWLINE,

//         TokenType::KW_DONE,     TokenType::NEWLINE,

//         TokenType::IDENTIFIER,  TokenType::LPAREN,     TokenType::IDENTIFIER,   TokenType::RPAREN,
//         TokenType::NEWLINE,

//         TokenType::IDENTIFIER,  TokenType::LPAREN,     TokenType::CHAR_LITERAL, TokenType::RPAREN,
//         TokenType::END_OF_FILE } );
// };

TEST_F( LexerTest, TightCode ) {
  assertTokensType( "int x=5_3+3(x)[x]\"abcd\"", {
                                                     TokenType::T_INT,
                                                     TokenType::IDENTIFIER,
                                                     TokenType::OP_ASSIGN,
                                                     TokenType::INT_LITERAL,
                                                     TokenType::OP_PLUS,
                                                     TokenType::INT_LITERAL,
                                                     TokenType::LPAREN,
                                                     TokenType::IDENTIFIER,
                                                     TokenType::RPAREN,
                                                     TokenType::LBRACKET,
                                                     TokenType::IDENTIFIER,
                                                     TokenType::RBRACKET,
                                                     TokenType::STRING_LITERAL,
                                                     TokenType::END_OF_FILE,
                                                 } );
}
