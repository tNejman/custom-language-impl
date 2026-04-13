#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Lexer/Lexer.h"

class LexerTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

Lexer assertTokensTypeAndPos(const std::string&& code, const std::vector<Token>&& expected) {
  std::stringstream ss(code);
  Lexer lexer(ss);
  for (const auto& exp : expected) {
    ASSERT_EQ(exp, lexer.getNextToken());
  }
  return lexer;
}

Lexer assertTokensType(const std::string&& code, const std::vector<TokenType>&& expected) {
  std::stringstream ss(code);
  Lexer lexer(ss);
  for (const auto& exp : expected) {
    ASSERT_EQ(exp, lexer.getNextToken().type_);
  }
  return lexer;
}

TEST_F( LexerTest, check_init ) {
  ASSERT_TRUE( true );
}

TEST_F( LexerTest, check_1 ) {
  assertTokensTypeAndPos("int x = 5", {
    {TokenType::T_INT, {1, 0}},
    {TokenType::IDENTIFIER, {1, 5}},
    {TokenType::OP_ASSIGN, {1, 7}},
    {TokenType::INT_LITERAl, {1, 9}}
  });
}

TEST_F(LexerTest, VariableDeclarations) {
    assertTokensTypeAndPos("var counter = 0", {
        {{1, 0}, TokenType::KW_VAR},
        {{1, 4}, TokenType::IDENTIFIER},
        {{1, 12}, TokenType::OP_ASSIGN},
        {{1, 14}, TokenType::INT_LITERAl}
    });

    assertTokensTypeAndPos("float pi = 3.14_15_926", {
        {{1, 0}, TokenType::T_FLOAT},
        {{1, 6}, TokenType::IDENTIFIER},
        {{1, 9}, TokenType::OP_ASSIGN},
        {{1, 11}, TokenType::FLOAT_LITERAL}
    });
}

TEST_F(LexerTest, Literals) {
    assertTokensTypeAndPos("char newline = '\\n'", {
        {{1, 0}, TokenType::T_CHAR},
        {{1, 5}, TokenType::IDENTIFIER},
        {{1, 13}, TokenType::OP_ASSIGN},
        {{1, 15}, TokenType::CHAR_LITERAL},
        {{1, 21}, TokenType::END_OF_FILE}
    });

    assertTokensTypeAndPos("bool is_ready = true", {
        {{1, 0}, TokenType::T_BOOL},
        {{1, 5}, TokenType::IDENTIFIER},
        {{1, 14}, TokenType::OP_ASSIGN},
        {{1, 16}, TokenType::BOOL_LITERAL},
        {{1, 20}, TokenType::END_OF_FILE}
    });
}

TEST_F(LexerTest, FunctionDefinition) {
    assertTokensType(
      "def int calculate(int x, int y) do
          ret x + y
       done", {
        TokenType::KW_DEF,
        TokenType::T_INT,
        TokenType::IDENTIFIER,
        TokenType::LPAREN,
        TokenType::T_INT,
        TokenType::IDENTIFIER,
        TokenType::COMMA,
        TokenType::T_INT,
        TokenType::IDENTIFIER,
        TokenType::RPAREN,
        TokenType::KW_DO,
        TokenType::NEWLINE,

        TokenType::KW_RET,
        TokenType::IDENTIFIER,
        TokenType::OP_PLUS,
        TokenType::IDENTIFIER,
        TokenType::NEWLINE,

        TokenType::KW_DONE
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, ControlFlow) {
    assertTokensType("
      if (a >= 10 and b <= 20) do
          a += 1
      done elseif (a != b or not c) do
          b -= 1
      done else do
          a = 2
          b /= 2
      done", {
        TokenType::KW_IF,
        TokenType::LPAREN,
        TokenType::IDENTIFIER,
        TokenType::OP_GEQ,
        TokenType::INT_LITERAL,
        TokenType::OP_AND,
        TokenType::IDENTIFIER,
        TokenType::OP_LEQ,
        TokenType::INT_LITERAL,
        TokenType::RPAREN,
        TokenType::KW_DO,
        TokenType::NEWLINE,

        TokenType::KW_DONE,
        TokenType::KW_ELSEIF,
        TokenType::LPAREN,
        TokenType::IDENTIFIER,
        TokenType::OP_NEQ,
        TokenType::IDENTIFIER,
        TokenType::OP_OR,
        TokenType::OP_NOT,
        TokenType::IDENTIFIER,
        TokenType::RPAREN,
        TokenType::KW_DO,
        TokenType::NEWLINE,

        TokenType::IDENTIFIER,
        TokenType::OP_SUB_ASSIGN,
        TokenType::INT_LITERAL,
        TokenType::NEWLINE,

        TokenType::KW_DONE,
        TokenType::KW_ELSE,
        TokenType::KW_DO,
        TokenType::NEWLINE,

        TokenType::IDENTIFIER,
        TokenType::OP_ASSIGN,
        TokenType::INT_LITERAL,
        TokenType::NEWLINE,

        TokenType::IDENTIFIER,
        TokenType::OP_DIV_ASSIGN,
        TokenType::INT_LITERAL,
        TokenType::NEWLINE,

        TokenType::KW_DONE,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, ConcatenationInitialization) {
    AssertTokensType("var [int] arr = [1,2,3] ++ [4,5]", {
        TokenType::KW_VAR,
        TokenType::LBRACKET,
        TokenType::T_INT,
        TokenType::RBRACKET,
        TokenType::IDENTIFIER,
        TokenType::OP_ASSIGN,
        TokenType::LBRACKET,
        TokenType::INT_LITERAL,
        TokenType::COMMA,
        TokenType::INT_LITERAL,
        TokenType::COMMA,
        TokenType::INT_LITERAL,
        TokenType::RBRACKET,
        TokenType::OP_CONCAT,
        TokenType::LBRACKET,
        TokenType::INT_LITERAL,
        TokenType::COMMA,
        TokenType::INT_LITERAL,
        TokenType::RBRACKET,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, OperatorDiff) {
    AssertTokensType("arr = arr -- [2]", {
        TokenType::IDENTIFIER,
        TokenType::OP_ASSIGN,
        TokenType::IDENTIFIER,
        TokenType::OP_DIFF,
        TokenType::LBRACKET,
        TokenType::INT_LITERAL,
        TokenType::RBRACKET,
    });
}

TEST_F(LexerTest, OperatorLength) {
    AssertTokensType("var int size = $arr", {
        TokenType::KW_VAR,
        TokenType::T_INT,
        TokenType::IDENTIFIER,
        TokenType::OP_ASSIGN,
        TokenType::OP_LEN,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, OperatorFilter) {
    AssertTokensType("var [int] filtered = arr ? isBig", {
        TokenType::KW_VAR,
        TokenType::LBRACKET,
        TokenType::T_INT,
        TokenType::RBRACKET,
        TokenType::IDENTIFIER,
        TokenType::OP_ASSIGN,
        TokenType::IDENTIFIER,
        TokenType::OP_FILTER,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, OperatorReverse) {
  AssertTokensType("[int] reversed = @arr", {
    TokenType::LBRACKET,
    TokenType::T_INT,
    TokenType::RBRACKET,
    TokenType::IDENTIFIER,
    TokenType::OP_ASSIGN,
    TokenType::OP_REV,
    TokenType::IDENTIFIER,
    TokenType::END_OF_FILE
  });
}

TEST_F(LexerTest, WhileBreakContinue) {
  assertTokensType("
    while (i > 0) do
      if (i == 10) do
        break
      done elseif (i == 17) do
        continue
      done
    done
    "), {
      TokenType::KW_WHILE,
      TokenType::LPAREN,
      TokenType::IDENTIFIER,
      TokenType::OP_GT,
      TokenType::INT_LITERAL,
      TokenType::RPAREN,
      TokenType::KW_DO,
      TokenType::NEWLINE

      TokenType::KW_IF,
      TokenType::LPAREN,
      TokenType::OP_EQ,
      TokenType::INT_LITERAL,
      TokenType::RPAREN,
      TokenType::KW_DO,
      TokenType::NEWLINE,

      TokenType::KW_BREAK,
      TokenType::NEWLINE,

      TokenType::KW_DONE,
      TokenType::KW_ELSEIF,
      TokenType::LPAREN,
      TokenType::IDENTIFIER,
      TokenType::OP_EQ,
      TokenType::RPAREN,
      TokenType::KW_DO,
      TokenType::NEWLINE,

      TokenType::KW_CONTINUE,
      TokenType::NEWLINE,

      TokenType::KW_DONE,
      TokenType::NEWLINE,

      TokenType::KW_DONE,
      TokenType::END_OF_FILE
    }
}

TEST_F(LexerTest, FullLineComment) {
  assertTokensType("
    # this is a full line comment
    # int x = 5
    # previous line was also a comment
    ", {
      TokenType::COMMENT,
      TokenType::NEWLINE,
      TokenType::COMMENT,
      TokenType::NEWLINE,
      TokenType::COMMENT,
      TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, CommentAfterCode) {
    AssertTokensType("int x = 10 # comment", {
        TokenType::T_INT,
        TokenType::IDENTIFIER,
        TokenType::OP_ASSIGN,
        TokenType::INT_LITERAl,
        TokenType::COMMENT,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, Whitespaces) {
    AssertTokensType("            ", {
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, IdentifiersAndNumbers) {
    AssertTokensType("1_000", {TokenType::INT_LITERAl});
    AssertTokensType("1____000", {TokenType::INT_LITERAL});
    AssertTokensType("12.34", {TokenType::FLOAT_LITERAL});
}

TEST_F(LexerTest, OperatorChains) {
    AssertTokensType("+=-==/=++", {
        TokenType::OP_ADD_ASSIGN,
        TokenType::OP_SUB_ASSIGN,
        TokenType::OP_ASSIGN,
        TokenType::OP_DIV_ASSIGN,
        TokenType::OP_CONCAT
    });
}

TEST_F(LexerTest){
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

  assertTokensType("
    [int] v = [1, 2, 5]
    int x = 5
    var int l = 0
    var int r = $v - 1
    while (l < r) do
      int mid = (l + r) / 2
      if (v[mid] <= x) do
        l = mid
      done else do
        r = mid - 1
      done
    done
    write(r)
    write('\n')
    "), {
      TokenType::LBRACKET, TokenType::T_INT, TokenType::RBRACKET, TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::LBRACKET,
      TokenType::INT_LITERAL, TokenType::COMMA, TokenType::INT_LITERAL, TokenType::COMMA, TokenType::INT_LITERAL,
      TokenType::RBRACKET, TokenType::NEWLINE,

      TokenType::T_INT, TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::INT_LITERAL, TokenType::NEWLINE,
      TokenType::KW_VAR, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::INT_LITERAL, TokenType::NEWLINE,
      TokenType::KW_VAR, TokenType::T_INT, TokenType::IDENTIFIER, TokenType::OP_ASSIGN,
          TokenType::OP_LEN, TokenType::IDENTIFIER, TokenType::OP_MINUS, TokenType::INT_LITERAL, TokenType::NEWLINE
      TokenType::KW_WHILE TokenType::LPAREN, TokenType::IDENTIFIER, TokenType::OP_LTE,
          TokenType::IDENTIFIER, TokenType::RPAREN, TokenType::KW_DO, TokenType::NEWLINE,
      TokenType::T_INT, TokenType::IDENTIFIER, TokenType::LPAREN, TokenType::IDENTIFIER, TokenType::OP_PLUS,
          TokenType::IDENTIFIER, TokenType::RPAREN, TokenType::OP_DIV, TokenType::INT_LITERAL, TokenType::NEWLINE,
      TokenType::KW_IF, TokenType::LPAREN, TokenType::IDENTIFIER, TokenType::LBRACKET, TokenType::IDENTIFIER, TokenType::RBRACKET,
          TokenType::OP_LEQ, TokenType::IDENTIFIER, TokenType::RPAREN, TokenType::KW_DO, TokenType::NEWLINE,
      TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::IDENTIFIER, TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::KW_ELSE, TokenType::KW_DO, TokenType::NEWLINE,
      TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::IDENTIFIER, TokenType::OP_MINUS, TokenType::INT_LITERAL, TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
      TokenType::IDENTIFIER, TokenType::LPAREN, TokenType::IDENTIFIER, TokenType::RPAREN, TokenType::NEWLINE,
      TokenType::IDENTIFIER, TokenType::LPAREN, TokenType::CHAR_LITERAL, TokenType::RPAREN, TokenType::END_OF_FILE
    }
}