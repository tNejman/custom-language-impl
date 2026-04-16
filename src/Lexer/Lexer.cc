#include "Lexer/Lexer.h"

void Lexer::nextChar() {
  if ( input_stream_.get( current_char_ ) ) {
    if ( current_char_ == '\n' ) {
      this->current_pos_.line_ += 1;
      this->current_pos_.column_ = 1;
    } else {
      this->current_pos_.column_ += 1;
    }
  } else {
    if ( !is_eof_ ) {
      this->current_pos_.column_ += 1;
      is_eof_ = true;
    }
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

std::string Lexer::buildStringLiteral() {
  std::string str;
  str.reserve( MAX_STRING_LITERAL_LENGTH + 1 );
  unsigned int str_literal_length = 0u;
  while ( current_char_ != '\"' && current_char_ != '\n' ) {
    // @TODO handle escapable characters
    str.push_back( current_char_ );
    ++str_literal_length;
    nextChar();

    if ( str_literal_length > MAX_STRING_LITERAL_LENGTH ) {
      throw TooLongStringLiteralException( "" );
    }
  }
  if ( current_char_ == '\n' ) {
    throw UnterminatedStringLiteralException( "" );
  }
  return std::string{ str.begin(), str.end() };
}

char Lexer::buildCharLiteral() {
  char c;
  if ( current_char_ == '\\' ) {
    nextChar();
    c = buildEscapeCharacter( current_char_ );
  } else {
    c = current_char_;
  }
  nextChar();
  if ( current_char_ != '\'' ) {
    throw UnterminatedCharLiteralException( "" );
  }
  nextChar();

  return c;
}

char Lexer::buildEscapeCharacter( const char c ) const {
  switch ( c ) {
    case 'n':
      return '\n';
    case 't':
      return '\t';
    case '\\':
      return '\\';
    case '\"':
      return '\"';
    default:
      throw UnknownEscapedCharacterException( "" );
  }
}

std::variant<int, float> Lexer::buildNumericLiteral() {
  std::string buf = "";
  bool is_float = false;

  while ( isDigit( current_char_ ) || current_char_ == '_' ) {
    if ( current_char_ != '_' ) {
      buf += current_char_;
    }
    nextChar();
  }

  if ( current_char_ == '.' ) {
    is_float = true;
    buf += current_char_;
    nextChar();

    while ( isDigit( current_char_ ) || current_char_ == '_' ) {
      if ( current_char_ != '_' ) {
        buf += current_char_;
      }
      nextChar();
    }
  }

  if ( is_float ) {
    try {
      return std::stof( buf );
    } catch ( const std::out_of_range& ) {
      throw MalformedNumericLiteralException( "" );
    }
  } else {
    try {
      return std::stoi( buf );
    } catch ( const std::out_of_range& ) {
      throw IntLiteralOutOfBoundsException( "" );
    }
  }
}

std::string Lexer::buildIdentifier() {
  std::string buf = "";
  // buf.reserve( MAX_IDENTIFIER_LENGTH + 1 );
  while ( isLetter( current_char_ ) || current_char_ == '_' ) {
    buf.push_back( current_char_ );
    nextChar();

    if ( buf.size() > MAX_IDENTIFIER_LENGTH ) {
      throw TooLongIdentifierException( "" );
    }
  }
  return buf;
}

TokenType Lexer::getSpecialIdentifierType( const std::string& identifier ) const {
  static std::map<std::string, TokenType> keywords = {
      { { "if", TokenType::KW_IF },           { "elseif", TokenType::KW_ELSEIF },
        { "else", TokenType::KW_ELSE },       { "while", TokenType::KW_WHILE },
        { "do", TokenType::KW_DO },           { "done", TokenType::KW_DONE },
        { "break", TokenType::KW_BREAK },     { "continue", TokenType::KW_CONTINUE },
        { "ret", TokenType::KW_RET },         { "def", TokenType::KW_DEF },
        { "var", TokenType::KW_VAR },         { "copy", TokenType::KW_COPY },
        { "cast_to", TokenType::KW_CAST_TO }, { "mut", TokenType::KW_MUT },
        { "bool", TokenType::T_BOOL },        { "char", TokenType::T_CHAR },
        { "float", TokenType::T_FLOAT },      { "int", TokenType::T_INT },
        { "str", TokenType::T_STR },          { "void", TokenType::T_VOID },
        { "true", TokenType::BOOL_LITERAL },  { "false", TokenType::BOOL_LITERAL },
        { "and", TokenType::OP_AND },         { "or", TokenType::OP_OR },
        { "not", TokenType::OP_NOT } } };

  auto it = keywords.find( identifier );

  if ( it != keywords.end() ) {
    return it->second;
  }

  return TokenType::IDENTIFIER;
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
    // * Jednoznaki: \n $ ? @ ( ) [ ] : , %
    case '\n':
      nextChar();
      return Token{ start_pos, TokenType::NEWLINE };
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
      nextChar();
      return Token{ start_pos, TokenType::STRING_LITERAL, buildStringLiteral() };
    case '\'':
      nextChar();
      return Token{ start_pos, TokenType::CHAR_LITERAL, buildCharLiteral() };
    case '#':
      while ( current_char_ != '\n' && current_char_ != '\0' ) nextChar();
      return Token{ start_pos, TokenType::COMMENT };
    default:
      break;
  }

  if ( isDigit( this->current_char_ ) ) {
    std::variant<int, float> lit = buildNumericLiteral();
    try {
      return Token{ start_pos, TokenType::INT_LITERAL, std::get<int>( lit ) };
    } catch ( std::bad_variant_access& ) {
      return Token{ start_pos, TokenType::FLOAT_LITERAL, std::get<float>( lit ) };
    }
  }

  if ( isLetter( this->current_char_ ) ) {
    std::string identifier = buildIdentifier();
    TokenType type = getSpecialIdentifierType( identifier );
    if ( type == TokenType::IDENTIFIER ) {
      return Token{ start_pos, type, identifier };
    } else if ( type == TokenType::BOOL_LITERAL ) {
      bool bool_val = identifier == "true";
      return Token{ start_pos, type, bool_val };
    } else {
      return Token{ start_pos, type };
    }
  }

  throw UnknownSymbolException( "" );
}