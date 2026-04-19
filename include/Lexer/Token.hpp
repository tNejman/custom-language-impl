#pragma once

#include <format>
#include <magic_enum/magic_enum.hpp>
#include <ostream>
#include <string>
#include <variant>

constexpr unsigned int MAX_IDENTIFIER_LENGTH = 128u;
constexpr unsigned int MAX_STRING_LITERAL_LENGTH = 128u;

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

  COLON,
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

inline std::ostream &operator<<( std::ostream &stream, const TokenType &tt ) {
  stream << magic_enum::enum_name( tt );
  return stream;
}

struct Position {
  unsigned int line_;
  unsigned int column_;

  auto operator==( const Position &other ) const {
    return this->line_ == other.line_ && this->column_ == other.column_;
  }
  bool operator==( const std::initializer_list<unsigned int> &other ) const {
    return other.size() == 2 && this->line_ == *( other.begin() ) && this->column_ == *( other.begin() + 1 );
  }

  std::string toString() const {
    return std::format( "Line: {}, Column: {}.", this->line_, this->column_ );
  }
  friend std::ostream &operator<<( std::ostream &stream, const Position &position ) {
    return stream << position.toString();
  }
};

using TokenVal = std::variant<std::monostate, int, float, bool, char, std::string>;

struct Token {
  Position position_;
  TokenType type_;
  TokenVal value_;

  bool operator==( const Token &other ) const {
    return this->position_ == other.position_ && this->type_ == other.type_;
  }
  friend std::ostream &operator<<( std::ostream &stream, const Token &token ) {
    // @TODO maybe replace with std::visit
    stream << "Token repr. Type: " << token.type_ << ", Position: " << token.position_;
    if ( token.type_ == TokenType::STRING_LITERAL || token.type_ == TokenType::IDENTIFIER
         || token.type_ == TokenType::COMMENT ) {
      stream << "Value: " << std::get<std::string>( token.value_ );
    } else if ( token.type_ == TokenType::CHAR_LITERAL ) {
      stream << "Value: " << std::get<char>( token.value_ );
    } else if ( token.type_ == TokenType::INT_LITERAL ) {
      stream << "Value: " << std::get<int>( token.value_ );
    } else if ( token.type_ == TokenType::FLOAT_LITERAL ) {
      stream << "Value: " << std::get<float>( token.value_ );
    } else if ( token.type_ == TokenType::BOOL_LITERAL ) {
      stream << "Value: " << std::get<bool>( token.value_ );
    }

    return stream;
  }
};