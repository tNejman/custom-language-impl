#pragma once

#include <istream>
#include <string>

#include "Lexer/ILexer.h"

class Lexer : public ILexer {
 private:
  std::istream &input_stream_;
  Position current_pos_{ 1, 0 };
  Position start_pos_{ current_pos_ };
  char current_char_{ ' ' };
  bool is_eof_ = false;

  Token makeToken( TokenType type ) const;
  Token makeToken( TokenType type, TokenVal value ) const;

  void nextChar();
  bool isWhiteSpace( const char c ) const;
  bool isDigit( const char c ) const;
  bool isLetter( const char c ) const;

  std::string buildStringLiteral();
  char buildCharLiteral();
  char buildEscapeCharacter( const char c ) const;
  std::variant<int, float> buildNumericLiteral();
  std::string buildComment();

  std::string buildIdentifier();
  TokenType getSpecialIdentifierType( const std::string &identifier ) const;

 public:
  explicit Lexer( std::istream &input );

  Token getNextToken() override;
};