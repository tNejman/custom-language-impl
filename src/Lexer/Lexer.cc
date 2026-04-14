#include "Lexer/Lexer.h"

void Lexer::nextChar() {
  if ( input_stream_.get( current_char_ ) ) {
    this->current_pos_.column_ += 1;
  } else {
    current_char_ = '\0';
  }
}

char Lexer::peek() const {
  // @TODO
  return '\0';
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

Lexer::Lexer( std::istream& input ) : input_stream_( input ) {
  nextChar();
}

Token Lexer::getNextToken() {
  while ( isWhiteSpace( this->current_char_ ) ) {
    nextChar();
  }
  if ( this->current_char_ == '\0' ) return Token{ this->current_pos_, TokenType::END_OF_FILE };

  Position start_pos = this->current_pos_;

  switch ( this->current_char_ ) {
    // * Jednoznaki: $ ? @ ( ) [ ] : , %
    case '$':
      nextChar();
      return Token{ start_pos, TokenType::OP_LEN };
    case '?':
      nextChar();
      return Token{ start_pos, TokenType::OP_FILTER };
    case '@':
      nextChar();
      return Token{ start_pos, TokenType::OP_REV };
    case '(':
      nextChar();
      return Token{ start_pos, TokenType::LPAREN };
    case ')':
      nextChar();
      return Token{ start_pos, TokenType::RPAREN };
    case '[':
      nextChar();
      return Token{ start_pos, TokenType::LBRACKET };
    case ']':
      nextChar();
      return Token{ start_pos, TokenType::RBRACKET };
    case ':':
      nextChar();
      return Token{ start_pos, TokenType::COLON };
    case ',':
      nextChar();
      return Token{ start_pos, TokenType::COMMA };
    case '%':
      nextChar();
      return Token{ start_pos, TokenType::OP_MOD };

      // * Wieloznaki i ich bazy + += ++ - -= -- -> < <= > >= = == * *= / /=
    case '+':
      nextChar();
      if ( current_char_ == '+' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_CONCAT };
      } else if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_ADD_ASSIGN };
      }
      return Token{ start_pos, TokenType::OP_PLUS };

    case '-':
      nextChar();
      if ( current_char_ == '-' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_DIFF };
      } else if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_SUB_ASSIGN };
      } else if ( current_char_ == '>' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_MAP };
      }
      return Token{ start_pos, TokenType::OP_MINUS };

    case '*':
      nextChar();
      if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_MUL_ASSIGN };
      }
      return Token{ start_pos, TokenType::OP_MUL };

    case '/':
      nextChar();
      if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_DIV_ASSIGN };
      }
      return Token{ start_pos, TokenType::OP_DIV };

    case '<':
      nextChar();
      if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_LEQ };
      }
      return Token{ start_pos, TokenType::OP_LT };

    case '>':
      nextChar();
      if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_GEQ };
      }
      return Token{ start_pos, TokenType::OP_GT };

    case '=':
      nextChar();
      if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_EQ };
      }
      return Token{ start_pos, TokenType::OP_ASSIGN };

    case '!':
      nextChar();
      if ( current_char_ == '=' ) {
        nextChar();
        return Token{ start_pos, TokenType::OP_NEQ };
      }
      throw UnknownSymbolException( "" );

    case '\"':
      // return buildStringLiteral();
    case '\'':
      // return buildCharLiteral();
    case '#':
      // make comment Token and drop everything until the end of line
  }

  if ( isDigit( this->current_char_ ) ) {
    // return buildNumericToken();
  }

  if ( isLetter( this->current_char_ ) ) {
    // make identifier then check for keywords
  }

  throw UnknownSymbolException( "" );
}