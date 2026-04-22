#include "Lexer/Lexer.h"

#include <unordered_map>

#include "Exceptions/LexerExceptions/FloatLiteralOutOfBoundsException.hpp"
#include "Exceptions/LexerExceptions/IntLiteralOutOfBoundsException.hpp"
#include "Exceptions/LexerExceptions/InvalidCharLiteralException.hpp"
#include "Exceptions/LexerExceptions/InvalidCommentStyleException.hpp"
#include "Exceptions/LexerExceptions/MalformedNumericLiteralException.hpp"
#include "Exceptions/LexerExceptions/TooLongIdentifierException.hpp"
#include "Exceptions/LexerExceptions/TooLongStringLiteralException.hpp"
#include "Exceptions/LexerExceptions/UnknownEscapedCharacterException.hpp"
#include "Exceptions/LexerExceptions/UnknownSymbolException.hpp"
#include "Exceptions/LexerExceptions/UnterminatedCharLiteralException.hpp"
#include "Exceptions/LexerExceptions/UnterminatedStringLiteralException.hpp"

Token Lexer::makeToken( TokenType type ) const {
  return Token{ start_pos_, type };
}

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

bool Lexer::isWhiteSpace( const char c ) const {
  if ( c == '\n' ) return false;
  return std::isspace( static_cast<unsigned char>( c ) );
}

bool Lexer::isDigit( const char c ) const {
  return c >= '0' && c <= '9';
}

bool Lexer::isLetter( const char c ) const {
  return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
}

std::string Lexer::buildStringLiteral() {
  std::string str{};
  if ( current_char_ == '\"' && static_cast<char>( input_stream_.peek() ) == '\"' ) {
    nextChar();
    return str;
  }
  while ( static_cast<char>( input_stream_.peek() ) != '\"' && static_cast<char>( input_stream_.peek() ) != '\n'
          && input_stream_.peek() != -1 ) {
    nextChar();
    if ( current_char_ == '\\' ) {
      if ( input_stream_.peek() == -1 || static_cast<char>( input_stream_.peek() ) == '\n' ) break;
      str.push_back( buildEscapeCharacter() );
    } else {
      str.push_back( current_char_ );
    }

    if ( str.size() > MAX_STRING_LITERAL_LENGTH ) {
      throw TooLongStringLiteralException( start_pos_, str.substr( 0, 16 ) + "..." );
    }
  }
  if ( static_cast<char>( input_stream_.peek() ) == '\n' || input_stream_.peek() == -1 ) {
    throw UnterminatedStringLiteralException( start_pos_,
                                              str.substr( 0, std::min<size_t>(str.size() / 3, 16)  ) + "..." );
  }
  nextChar();
  return str;
}

char Lexer::buildCharLiteral() {
  char c;
  nextChar();
  if ( current_char_ == '\\' ) {
    c = buildEscapeCharacter();
  } else if ( current_char_ == '\'' ) {
    throw InvalidCharLiteralException( start_pos_ );
  } else {
    c = current_char_;
  }
  nextChar();
  if ( current_char_ != '\'' ) {
    throw UnterminatedCharLiteralException( start_pos_, current_char_ );
  }
  return c;
}

char Lexer::buildEscapeCharacter() {
  nextChar();
  switch ( current_char_ ) {
    case 'n': return '\n';
    case 't': return '\t';
    case '\\': return '\\';
    case '\"': return '\"';
    case '\'': return '\'';
    default: throw UnknownEscapedCharacterException( start_pos_, current_char_ );
  }
}

std::variant<int, float> Lexer::buildNumericLiteral() {
  std::string buf{ current_char_ };
  std::string buf_org{ current_char_ };
  bool is_float = false;

  auto consume_digits_and_underscores = [&]() {
    while ( isDigit( static_cast<char>( input_stream_.peek() ) ) || static_cast<char>( input_stream_.peek() ) == '_' ) {
      nextChar();
      if ( current_char_ != '_' ) {
        buf += current_char_;
      }
      buf_org += current_char_;
    }
  };

  consume_digits_and_underscores();

  if ( static_cast<char>( input_stream_.peek() ) == '.' ) {
    nextChar();
    is_float = true;
    buf += current_char_;

    consume_digits_and_underscores();
  }
  if ( isLetter( static_cast<char>( input_stream_.peek() ) ) || static_cast<char>( input_stream_.peek() ) == '.' ) {
    throw MalformedNumericLiteralException(start_pos_, current_pos_, current_char_, buf_org );
  }

  if ( is_float ) {
    try {
      return std::stof( buf );
    } catch ( const std::out_of_range & ) {
      throw FloatLiteralOutOfBoundsException( start_pos_, buf_org );
    }
  } else { 
    try {
      return std::stoi( buf );
    } catch ( const std::out_of_range & ) {
      throw IntLiteralOutOfBoundsException( start_pos_, buf_org );
    }
  }
}

std::string Lexer::buildComment() {
  nextChar();
  while ( current_char_ == ' ' ) nextChar();
  std::string buf{ current_char_ };
  while ( static_cast<char>( input_stream_.peek() ) != '\n' && input_stream_.peek() != -1 ) {
    nextChar();
    buf.push_back( current_char_ );
  }
  return buf;
}

std::string Lexer::buildIdentifier() {
  std::string buf{ current_char_ };
  while ( isLetter( static_cast<char>( input_stream_.peek() ) ) || isDigit( static_cast<char>( input_stream_.peek() ) )
          || static_cast<char>( input_stream_.peek() ) == '_' ) {
    nextChar();
    buf.push_back( current_char_ );

    if ( buf.size() > MAX_IDENTIFIER_LENGTH ) {
      throw TooLongIdentifierException( start_pos_, buf.substr( 0, 16 ) + "..." );
    }
  }
  return buf;
}

TokenType Lexer::getSpecialIdentifierType( const std::string &identifier ) const {
  static const std::unordered_map<std::string, TokenType> keywords = {
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

  auto it = keywords.find( identifier );

  if ( it != keywords.end() ) {
    return it->second;
  }

  return TokenType::IDENTIFIER;
}

Lexer::Lexer( std::istream &input ) : input_stream_( input ) {
}

Token Lexer::getNextToken() {
  nextChar();
  while ( isWhiteSpace( current_char_ ) ) {
    nextChar();
  }
  this->start_pos_ = this->current_pos_;

  Token t;
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
    case ':': return makeToken( TokenType::COLON );
    case ',': return makeToken( TokenType::COMMA );

    case '+':
      if ( static_cast<char>( input_stream_.peek() ) == '+' ) {
        nextChar();
        return makeToken( TokenType::OP_CONCAT );
      } else if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_ADD_ASSIGN );
      }
      return makeToken( TokenType::OP_PLUS );

    case '-':
      if ( static_cast<char>( input_stream_.peek() ) == '-' ) {
        nextChar();
        return makeToken( TokenType::OP_DIFF );
      } else if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_SUB_ASSIGN );
      } else if ( static_cast<char>( input_stream_.peek() ) == '>' ) {
        nextChar();
        return makeToken( TokenType::OP_MAP );
      }
      return makeToken( TokenType::OP_MINUS );

    case '*':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_MUL_ASSIGN );
      }
      return makeToken( TokenType::OP_MUL );

    case '/':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_DIV_ASSIGN );
      } else if ( static_cast<char>( input_stream_.peek() ) == '*' ) {
        throw InvalidCommentStyleException( current_pos_ );
      }
      return makeToken( TokenType::OP_DIV );

    case '%':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_MOD_ASSIGN );
      }
      return makeToken( TokenType::OP_MOD );

    case '<':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_LEQ );
      }
      return makeToken( TokenType::OP_LT );

    case '>':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_GEQ );
      }
      return makeToken( TokenType::OP_GT );

    case '=':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_EQ );
      }
      return makeToken( TokenType::OP_ASSIGN );

    case '!':
      if ( static_cast<char>( input_stream_.peek() ) == '=' ) {
        nextChar();
        return makeToken( TokenType::OP_NEQ );
      }
      throw UnknownSymbolException( current_pos_, current_char_ );

    case '\"': return makeToken( TokenType::STRING_LITERAL, buildStringLiteral() );
    case '\'': return makeToken( TokenType::CHAR_LITERAL, buildCharLiteral() );
    case '#': return makeToken( TokenType::COMMENT, buildComment() );
    default: break;
  }

  if ( isDigit( this->current_char_ ) ) {
    std::variant<int, float> lit = buildNumericLiteral();
    if ( auto *i = std::get_if<int>( &lit ) ) {
      return makeToken( TokenType::INT_LITERAL, std::get<int>( lit ) );
    } else {
      return makeToken( TokenType::FLOAT_LITERAL, std::get<float>( lit ) );
    }
  }

  else if ( isLetter( this->current_char_ ) ) {
    std::string identifier = buildIdentifier();
    TokenType type = getSpecialIdentifierType( identifier );
    if ( type == TokenType::IDENTIFIER ) {
      return makeToken( type, identifier );
    } else if ( type == TokenType::BOOL_LITERAL ) {
      bool bool_val = identifier == "true";
      return makeToken( type, bool_val );
    } else {
      return makeToken( type );
    }
  }

  throw UnknownSymbolException( current_pos_, current_char_ );
}