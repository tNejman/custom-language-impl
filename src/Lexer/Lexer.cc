#include "Lexer/Lexer.h"

#include <array>
#include <cctype>
#include <limits>
#include <optional>
#include <ranges>
#include <string>

#include "Exceptions/LexerExceptions/_LexerExceptionInclude.hpp"


Token Lexer::makeToken( TokenType type, TokenVal value ) const {
  return Token{ start_pos_, type, std::move( value ) };
}

void Lexer::nextChar() {
  char previous = current_char_;
  while ( input_stream_.get( current_char_ ) ) {
    if ( current_char_ == '\0' )
      continue;
    else if ( current_char_ == '\r' ) {
      if ( input_stream_.peek() == '\n' ) {
        input_stream_.get();
      }
      current_char_ = '\n';
    }
    if ( previous == '\n' ) {
      this->current_pos_.line_ += 1;
      this->current_pos_.column_ = 1;
    } else {
      this->current_pos_.column_ += 1;
    }
    return;
  }

  if ( !is_eof_ ) {
    if ( current_char_ == '\n' ) {
      this->current_pos_.line_ += 1;
      this->current_pos_.column_ = 1;
    } else {
      this->current_pos_.column_ += 1;
    }
    this->is_eof_ = true;
  } else {
    this->start_pos_ = this->current_pos_;
  }
  current_char_ = '\0';
}

char Lexer::peek() const {
  return static_cast<char>( input_stream_.peek() );
}

void Lexer::skipWhitespaces() {
  while ( std::isblank( current_char_ ) ) nextChar();
}

std::optional<Token> Lexer::tryBuildDigraph( std::vector<std::pair<char, TokenType>> premade ) {
  for ( const auto &pair : premade ) {
    if ( pair.first == peek() ) {
      nextChar();
      return makeToken( pair.second );
    }
  }
  return std::nullopt;
}

std::optional<Token> Lexer::tryBuildIdentifier( const std::string &identifier ) const {
  static constexpr std::array<std::pair<std::string_view, TokenType>, 25> keywords = {
      { { "if", TokenType::KW_IF },           { "elseif", TokenType::KW_ELSEIF },
        { "else", TokenType::KW_ELSE },       { "while", TokenType::KW_WHILE },
        { "do", TokenType::KW_DO },           { "done", TokenType::KW_DONE },
        { "break", TokenType::KW_BREAK },     { "continue", TokenType::KW_CONTINUE },
        { "ret", TokenType::KW_RET },         { "def", TokenType::KW_DEF },
        { "var", TokenType::KW_VAR },         { "copy", TokenType::KW_COPY },
        { "cast_to", TokenType::KW_CAST_TO }, { "mut", TokenType::KW_MUT },
        { "bool", TokenType::T_BOOL },        { "char", TokenType::T_CHAR },
        { "float", TokenType::T_FLOAT },      { "int", TokenType::T_INT },
        { "string", TokenType::T_STR },       { "void", TokenType::T_VOID },
        { "true", TokenType::BOOL_LITERAL },  { "false", TokenType::BOOL_LITERAL },
        { "and", TokenType::OP_AND },         { "or", TokenType::OP_OR },
        { "not", TokenType::OP_NOT } } };

  auto it = std::ranges::find( keywords, identifier, []( const auto &p ) { return p.first; } );

  if ( it == keywords.end() ) return std::nullopt;
  if ( it->second == TokenType::BOOL_LITERAL ) {
    return makeToken( it->second, identifier == "true" );
  }
  return makeToken( it->second );
}

char Lexer::buildEscapeCharacter( const char escaped_char ) const {
  switch ( escaped_char ) {
    case 'n': return '\n';
    case 't': return '\t';
    case '\\': return '\\';
    case '\"': return '\"';
    case '\'': return '\'';
    default: throw UnknownEscapedCharacterException( current_pos_, current_char_ );
  }
}

std::optional<Token> Lexer::tryBuildSymbol() {
  switch ( this->current_char_ ) {
    case '\0': return makeToken( TokenType::END_OF_FILE );
    case '\n': return makeToken( TokenType::NEWLINE );
    case '$': return makeToken( TokenType::OP_LEN );
    case '?': return makeToken( TokenType::OP_FILTER );
    case '@': return makeToken( TokenType::OP_REV );
    case '(': return makeToken( TokenType::LPAREN );
    case ')': return makeToken( TokenType::RPAREN );
    case '[': return makeToken( TokenType::LBRACKET );
    case ']': return makeToken( TokenType::RBRACKET );
    case ',': return makeToken( TokenType::COMMA );
    case '+': {
      if ( auto m_token = tryBuildDigraph( { { '+', TokenType::OP_CONCAT }, { '=', TokenType::OP_ADD_ASSIGN } } ) )
        return m_token;
      return makeToken( TokenType::OP_PLUS );
    }
    case '-': {
      if ( auto m_token = tryBuildDigraph(
               { { '-', TokenType::OP_DIFF }, { '=', TokenType::OP_SUB_ASSIGN }, { '>', TokenType::OP_MAP } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_MINUS );
    }
    case '*': {
      if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_MUL_ASSIGN } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_MUL );
    }
    case '/': {
      if ( peek() == '*' )
        throw InvalidCommentStyleException( current_pos_ );
      else if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_DIV_ASSIGN } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_DIV );
    }
    case '%': {
      if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_MOD_ASSIGN } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_MOD );
    }
    case '<': {
      if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_LEQ } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_LT );
    }
    case '>': {
      if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_GEQ } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_GT );
    }
    case '=': {
      if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_EQ } } ) )
        return m_token;
      else
        return makeToken( TokenType::OP_ASSIGN );
    }
    case '!': {
      if ( auto m_token = tryBuildDigraph( { { '=', TokenType::OP_NEQ } } ) ) return m_token;
      throw UnknownSymbolException( current_pos_, current_char_ );
    }
    default: break;
  }
  return std::nullopt;
}

std::optional<Token> Lexer::tryBuildStringLiteral() {
  std::string str{};
  if ( current_char_ != '\"' ) return std::nullopt;

  while ( peek() != '\"' && peek() != '\n' && input_stream_.peek() != -1 ) {
    nextChar();
    if ( current_char_ == '\\' ) {
      if ( input_stream_.peek() == -1 || peek() == '\n' ) break;
      nextChar();
      str.push_back( buildEscapeCharacter( current_char_ ) );
    } else {
      str.push_back( current_char_ );
    }

    if ( str.size() > MAX_STRING_LITERAL_LENGTH ) {
      throw TooLongStringLiteralException( start_pos_, str.substr( 0, 16 ) + "..." );
    }
  }
  if ( peek() == '\n' || input_stream_.peek() == -1 ) {
    throw UnterminatedStringLiteralException( start_pos_,
                                              str.substr( 0, std::min<size_t>( str.size() / 3, 16 ) ) + "..." );
  }
  nextChar();
  return makeToken( TokenType::STRING_LITERAL, std::move( str ) );
}

std::optional<Token> Lexer::tryBuildCharLiteral() {
  if ( current_char_ != '\'' ) return std::nullopt;

  char char_literal_value = UNINITIALIZED_CHAR_MARKER;
  nextChar();
  if ( current_char_ == '\\' ) {
    nextChar();
    char_literal_value = buildEscapeCharacter( current_char_ );
  } else if ( current_char_ == '\'' ) {
    throw InvalidCharLiteralException( current_pos_ );
  } else {
    char_literal_value = current_char_;
  }
  nextChar();
  if ( current_char_ != '\'' ) {
    throw UnterminatedCharLiteralException( start_pos_, current_char_ );
  }
  return makeToken( TokenType::CHAR_LITERAL, char_literal_value );
}

std::optional<Token> Lexer::tryBuildNumericLiteral() {
  if ( !isdigit( current_char_ ) ) return std::nullopt;

  std::string buf{ current_char_ };
  std::string buf_org{ current_char_ };
  bool is_float = false;

  int int_val = current_char_ - '0';
  bool is_overflown = false;

  constexpr int max_div_10 = std::numeric_limits<int>::max() / 10;
  constexpr int max_mod_10 = std::numeric_limits<int>::max() % 10;

  auto consume_digits_and_underscores = [&]() {
    while ( isdigit( peek() ) || peek() == '_' ) {
      nextChar();
      buf_org += current_char_;

      if ( current_char_ != '_' ) {
        buf += current_char_;

        if ( !is_float && !is_overflown ) {
          int cur_subdec_val = current_char_ - '0';

          if ( int_val > max_div_10 || ( int_val == max_div_10 && cur_subdec_val > max_mod_10 ) ) {
            is_overflown = true;
          } else {
            int_val = int_val * 10 + cur_subdec_val;
          }
        }
      }
      buf_org += current_char_;
    }
  };

  consume_digits_and_underscores();

  if ( peek() == '.' ) {
    nextChar();
    is_float = true;
    buf += current_char_;
    buf_org += current_char_;

    consume_digits_and_underscores();
  }

  if ( isalpha( peek() ) || peek() == '.' ) {
    nextChar();
    throw MalformedNumericLiteralException( current_pos_, current_char_, std::move( buf_org ) );
  }

  if ( is_float ) {
    try {
      return makeToken( TokenType::FLOAT_LITERAL, std::stof( buf ) );
    } catch ( const std::out_of_range & ) {
      throw FloatLiteralOutOfBoundsException( start_pos_, buf_org );
    }
  } else {
    if ( is_overflown ) {
      throw IntLiteralOutOfBoundsException( start_pos_, buf_org );
    }
    return makeToken( TokenType::INT_LITERAL, int_val );
  }
}

std::optional<Token> Lexer::tryBuildComment() {
  if ( current_char_ != '#' ) return std::nullopt;
  nextChar();
  std::string buf{ current_char_ };
  while ( peek() != '\n' && input_stream_.peek() != -1 ) {
    nextChar();
    buf.push_back( current_char_ );
  }
  return makeToken( TokenType::COMMENT, std::move( buf ) );
}

std::optional<Token> Lexer::tryBuildIdentifier() {
  if ( !isalpha( current_char_ ) ) return std::nullopt;

  std::string buf{ current_char_ };
  while ( isalnum( peek() ) || peek() == '_' ) {
    nextChar();
    buf.push_back( current_char_ );

    if ( buf.size() > MAX_IDENTIFIER_LENGTH ) {
      throw TooLongIdentifierException( start_pos_, buf.substr( 0, 16 ) + "..." );
    }
  }

  auto token = tryBuildIdentifier( buf );
  if ( token == std::nullopt ) return makeToken( TokenType::IDENTIFIER, std::move( buf ) );
  return token;
}

Lexer::Lexer( std::istream &input ) : input_stream_( input ) {
}

Token Lexer::getNextToken() {
  nextChar();
  skipWhitespaces();
  this->start_pos_ = this->current_pos_;

  if ( auto t = tryBuildSymbol() ) return std::move( *t );
  if ( auto t = tryBuildStringLiteral() ) return std::move( *t );
  if ( auto t = tryBuildCharLiteral() ) return std::move( *t );
  if ( auto t = tryBuildNumericLiteral() ) return std::move( *t );
  if ( auto t = tryBuildComment() ) return std::move( *t );
  if ( auto t = tryBuildIdentifier() ) return std::move( *t );

  throw UnknownSymbolException( current_pos_, current_char_ );
}