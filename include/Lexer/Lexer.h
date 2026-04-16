#pragma once

#include <algorithm>
#include <array>
#include <istream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "Exceptions/LexerExceptions/IntLiteralOutOfBoundsException.hpp"
#include "Exceptions/LexerExceptions/InvalidCharLiteralException.hpp"
#include "Exceptions/LexerExceptions/MalformedNumericLiteralException.hpp"
#include "Exceptions/LexerExceptions/TooLongIdentifierException.hpp"
#include "Exceptions/LexerExceptions/TooLongStringLiteralException.hpp"
#include "Exceptions/LexerExceptions/UnknownEscapedCharacterException.hpp"
#include "Exceptions/LexerExceptions/UnknownSymbolException.hpp"
#include "Exceptions/LexerExceptions/UnterminatedCharLiteralException.hpp"
#include "Exceptions/LexerExceptions/UnterminatedStringLiteralException.hpp"
#include "Lexer/ILexer.h"

class Lexer : public ILexer {
 private:
  std::istream& input_stream_;
  Position current_pos_{ 1, 0 };
  char current_char_{ '\0' };
  bool is_eof_ = false;

  void nextChar();
  char peek() const;
  bool isWhiteSpace( const char c ) const;
  bool isDigit( const char c ) const;
  bool isLetter( const char c ) const;

  std::string buildStringLiteral();
  char buildCharLiteral();
  char buildEscapeCharacter( const char c ) const;
  std::variant<int, float> buildNumericLiteral();

  std::string buildIdentifier();
  TokenType getSpecialIdentifierType( const std::string& identifier ) const;

 public:
  explicit Lexer( std::istream& input );

  Token getNextToken() override;
};