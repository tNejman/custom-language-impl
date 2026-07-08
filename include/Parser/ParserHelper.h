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

bool isOperator( const TokenType token_type ) noexcept;

AssignmentType translateTokenTypeToAssignmentType( const TokenType token_type );
BinaryOperator translateTokenTypeToBinaryOperator( const TokenType token_type );
UnaryOperator translateTokenTypeToUnaryOperator( const TokenType token_type );
ArrayIdentifierOpType translateTokenTypeToArrayIdentifierOpType( const TokenType token_type );

std::string_view operatorToStr( const AssignmentType assignment_type );
std::string_view operatorToStr( const BinaryOperator binary_operator );
std::string_view operatorToStr( const UnaryOperator unary_operator );
};  // namespace parser_helper