#pragma once

enum class TokenType {
    COMMENT,
    INT_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    BOOL_LITERAL,

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

    OP_CONAT,
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

struct Position {
    unsigned int line_;
    unsigned int column_;

    bool operator==(const Position& other) const {
        return this->line_ == other.line_ && this->column_ == other.column_;
    }
};



struct Token {
    Position position_;
    TokenType type_;

    bool operator==(const Token& other) const {
        return this->position_ == other.position_ && this->type_ == other.type_;
    }
};