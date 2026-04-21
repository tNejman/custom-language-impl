#pragma once

#include <format>
#include <iomanip>
#include <magic_enum/magic_enum.hpp>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
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
    std::ostringstream type_ss;
    type_ss << token.type_ << ",";

    std::ostringstream pos_ss;
    pos_ss << token.position_;

    stream << std::left << "Token repr. Type: " << std::setw( 15 ) << type_ss.str() << " Position: " << std::setw( 25 )
           << pos_ss.str();

    std::visit(
        [&stream]( auto &&arg ) {
          using T = std::decay_t<decltype( arg )>;
          if constexpr ( !std::is_same_v<T, std::monostate> ) {
            stream << " Value: " << arg;
          };
        },
        token.value_ );

    stream << '\n';
    return stream;
  }
};