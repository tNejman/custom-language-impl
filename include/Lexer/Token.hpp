#pragma once

#include <cassert>
#include <string>
#include <variant>
#include <cmath>

constexpr int MAX_IDENTIFIER_LENGTH = 128u;
constexpr int MAX_STRING_LITERAL_LENGTH = 128u;

enum class TokenType : int {
  COMMENT,
  INT_LITERAL,
  FLOAT_LITERAL,
  CHAR_LITERAL,
  BOOL_LITERAL,
  STRING_LITERAL,

  T_INT,
  T_FLOAT,
  T_CHAR,
  T_STR,
  T_BOOL,
  T_VOID,

  KW_IF,
  KW_ELSEIF,
  KW_ELSE,
  KW_WHILE,
  KW_DO,
  KW_DONE,
  KW_BREAK,
  KW_CONTINUE,
  KW_RET,

  OP_AND,
  OP_OR,
  OP_NOT,

  KW_DEF,
  KW_VAR,
  KW_COPY,
  KW_CAST_TO,
  KW_MUT,

  OP_MAP,
  OP_EQ,
  OP_NEQ,
  OP_GEQ,
  OP_LEQ,

  OP_ADD_ASSIGN,
  OP_SUB_ASSIGN,
  OP_MUL_ASSIGN,
  OP_DIV_ASSIGN,
  OP_MOD_ASSIGN,

  OP_CONCAT,
  OP_DIFF,

  OP_ASSIGN,
  OP_PLUS,
  OP_MINUS,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_LEN,
  OP_FILTER,
  OP_GT,
  OP_LT,
  OP_REV,

  COMMA,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,

  IDENTIFIER,
  NEWLINE,
  WS,
  END_OF_FILE
};

struct Position {
  int line_;
  int column_;

  constexpr Position( const int line, const int column ) : line_( line ), column_( column ) {
    assert( line_ > 0 );
    assert( column_ >= 0 );
  }
  constexpr Position( const Position& ) noexcept = default;
  constexpr Position( Position&& ) noexcept = default;
  Position& operator=( const Position& ) noexcept = default;
  Position& operator=( Position&& ) noexcept = default;
  auto operator<=>( const Position& other ) const noexcept = default;
};

using TokenVal = std::variant<std::monostate, int, float, bool, char, std::string>;

struct Token {
  Position position_;
  TokenType type_;
  TokenVal value_;

  Token( Position position, TokenType type, TokenVal value = std::monostate{} )
      : position_( position ), type_( type ), value_( std::move( value ) ) {
    validate();
  }

  bool operator==( const Token& other ) const {
    if ( position_ != other.position_ || type_ != other.type_ ) {
      return false;
    }
    if ( std::holds_alternative<float>( value_ ) && std::holds_alternative<float>( other.value_ ) ) {
      constexpr float epsilon = 1e-5f;
      return std::abs( std::get<float>( value_ ) - std::get<float>( other.value_ ) ) < epsilon;
    }
    return value_ == other.value_;
  }

 private:
  void validate() const {
    switch ( type_ ) {
      case TokenType::INT_LITERAL: assert( std::holds_alternative<int>( value_ ) ); break;
      case TokenType::FLOAT_LITERAL: assert( std::holds_alternative<float>( value_ ) ); break;
      case TokenType::BOOL_LITERAL: assert( std::holds_alternative<bool>( value_ ) ); break;
      case TokenType::CHAR_LITERAL: assert( std::holds_alternative<char>( value_ ) ); break;
      case TokenType::STRING_LITERAL:
      case TokenType::COMMENT:
      case TokenType::IDENTIFIER: assert( std::holds_alternative<std::string>( value_ ) ); break;
      default: assert( std::holds_alternative<std::monostate>( value_ ) ); break;
    }
  }
};