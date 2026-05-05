#pragma once

#include "Lexer/Token.hpp"
#include "Parser/Node.h"

namespace parser_helper {
bool isAssignment( const TokenType token_type ) noexcept;
bool isEquality( const TokenType token_type ) noexcept;
bool isRelational( const TokenType token_type ) noexcept;
bool isAdditive( const TokenType token_type ) noexcept;
bool isMultiplicative( const TokenType token_type ) noexcept;
bool isUnary( const TokenType token_type ) noexcept;
bool isAccesExprSufBeg( const TokenType token_type ) noexcept;

bool isLiteral( const TokenType token_type ) noexcept;

AssignmentType translateTokenTypeToAssignmentType( const TokenType token_type ) noexcept;
BinaryOperator translateTokenTypeToBinaryOperator( const TokenType token_type ) noexcept;
UnaryOperator translateTokenTypeToUnaryOperator( const TokenType token_type ) noexcept;

};  // namespace parser_helper