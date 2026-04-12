#pragma once

enum class TokenType {

};

struct Position {
    unsigned int line_;
    unsigned int column_;
};

struct Token {
    Position position_;
    TokenType type_;
};