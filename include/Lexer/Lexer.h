#pragma once

#include <istream>
#include <optional>
#include <string>
#include <vector>

#include "Lexer/ILexer.h"

namespace {
constexpr Position STARTING_LEXER_POSITION{ 1, 0 };
constexpr char UNINITIALIZED_CHAR_MARKER = -2;
};  // namespace

class Lexer : public ILexer {
 private:
  std::istream &input_stream_;
  Position current_pos_{ STARTING_LEXER_POSITION };
  Position start_pos_{ current_pos_ };
  char current_char_{ UNINITIALIZED_CHAR_MARKER };
  bool is_eof_ = false;

  Token makeToken( TokenType type, TokenVal value = std::monostate{} ) const;

  void nextChar();
  char peek() const;
  void skipWhitespaces();
  std::optional<Token> tryBuildDigraph( std::vector<std::pair<char, TokenType>> premade );
  std::optional<Token> tryBuildIdentifier( const std::string &identifier ) const;
  char buildEscapeCharacter( const char escaped_char ) const;

  std::optional<Token> tryBuildSymbol();
  std::optional<Token> tryBuildStringLiteral();
  std::optional<Token> tryBuildCharLiteral();
  std::optional<Token> tryBuildNumericLiteral();
  std::optional<Token> tryBuildComment();
  std::optional<Token> tryBuildIdentifier();

 public:
  explicit Lexer( std::istream &input );

  Token getNextToken() override;
};