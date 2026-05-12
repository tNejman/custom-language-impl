#include "Parser/ParserHelper.h"

#include "Parser/Node.h"


namespace parser_helper {
bool isAssignment( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_ASSIGN:
    case TokenType::OP_ADD_ASSIGN:
    case TokenType::OP_SUB_ASSIGN:
    case TokenType::OP_DIV_ASSIGN:
    case TokenType::OP_MUL_ASSIGN:
    case TokenType::OP_MOD_ASSIGN: return true;
    default: return false;
  }
}

bool isEquality( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_EQ:
    case TokenType::OP_NEQ: return true;
    default: return false;
  }
}

bool isRelational( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_LT:
    case TokenType::OP_LEQ:
    case TokenType::OP_GT:
    case TokenType::OP_GEQ: return true;
    default: return false;
  }
}

bool isAdditive( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_PLUS:
    case TokenType::OP_MINUS:
    case TokenType::OP_CONCAT:
    case TokenType::OP_DIFF: return true;
    default: return false;
  }
}

bool isMultiplicative( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_MUL:
    case TokenType::OP_DIV:
    case TokenType::OP_MOD: return true;
    default: return false;
  }
}

bool isUnary( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_MINUS:
    case TokenType::OP_NOT:
    case TokenType::OP_REV:
    case TokenType::OP_LEN: return true;
    default: return false;
  }
}

bool isAccesExprSufBeg( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::LBRACKET:
    case TokenType::OP_FILTER:
    case TokenType::OP_MAP: return true;
    default: return false;
  }
}

bool isLiteral( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::BOOL_LITERAL:
    case TokenType::CHAR_LITERAL:
    case TokenType::FLOAT_LITERAL:
    case TokenType::INT_LITERAL:
    case TokenType::STRING_LITERAL: return true;
    default: return false;
  }
}

bool isOperator( const TokenType token_type ) noexcept {
  return isAssignment( token_type ) || isEquality( token_type ) || isRelational( token_type )
         || isAdditive( token_type ) || isMultiplicative( token_type ) || isUnary( token_type )
         || isAccesExprSufBeg( token_type );
}

AssignmentType translateTokenTypeToAssignmentType( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_ASSIGN: return AssignmentType::ASSIGN;
    case TokenType::OP_ADD_ASSIGN: return AssignmentType::ADD_ASSIGN;
    case TokenType::OP_SUB_ASSIGN: return AssignmentType::SUB_ASSIGN;
    case TokenType::OP_DIV_ASSIGN: return AssignmentType::MUL_ASSIGN;
    case TokenType::OP_MUL_ASSIGN: return AssignmentType::DIV_ASSIGN;
    case TokenType::OP_MOD_ASSIGN: return AssignmentType::MOD_ASSIGN;
  }
}

BinaryOperator translateTokenTypeToBinaryOperator( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_OR: return BinaryOperator::OR;
    case TokenType::OP_AND: return BinaryOperator::AND;
    case TokenType::OP_EQ: return BinaryOperator::EQ;
    case TokenType::OP_NEQ: return BinaryOperator::NEQ;
    case TokenType::OP_LT: return BinaryOperator::LT;
    case TokenType::OP_LEQ: return BinaryOperator::LEQ;
    case TokenType::OP_GT: return BinaryOperator::GT;
    case TokenType::OP_GEQ: return BinaryOperator::GEQ;
    case TokenType::OP_PLUS: return BinaryOperator::ADD;
    case TokenType::OP_MINUS: return BinaryOperator::SUB;
    case TokenType::OP_CONCAT: return BinaryOperator::CONCAT;
    case TokenType::OP_DIFF: return BinaryOperator::DIFF;
    case TokenType::OP_MUL: return BinaryOperator::MUL;
    case TokenType::OP_DIV: return BinaryOperator::DIV;
    case TokenType::OP_MOD: return BinaryOperator::MOD;
    case TokenType::OP_FILTER: return BinaryOperator::FILTER;
    case TokenType::OP_MAP: return BinaryOperator::MAP;
    case TokenType::KW_CAST_TO: return BinaryOperator::CAST_TO;
  }
}

UnaryOperator translateTokenTypeToUnaryOperator( const TokenType token_type ) noexcept {
  switch ( token_type ) {
    case TokenType::OP_MINUS: return UnaryOperator::NEG;
    case TokenType::OP_NOT: return UnaryOperator::NOT;
    case TokenType::OP_REV: return UnaryOperator::REV;
    case TokenType::OP_LEN: return UnaryOperator::LEN;
  }
}

std::string_view operatorToStr( const AssignmentType assignment_type ) noexcept {
  switch ( assignment_type ) {
    case AssignmentType::ASSIGN: return "=";
    case AssignmentType::ADD_ASSIGN: return "+=";
    case AssignmentType::SUB_ASSIGN: return "-=";
    case AssignmentType::MUL_ASSIGN: return "*=";
    case AssignmentType::DIV_ASSIGN: return "/=";
    case AssignmentType::MOD_ASSIGN: return "%=";
  }
}

std::string_view operatorToStr( const BinaryOperator binary_operator ) noexcept {
  switch ( binary_operator ) {
    case BinaryOperator::OR: return "or";
    case BinaryOperator::AND: return "and";
    case BinaryOperator::EQ: return "==";
    case BinaryOperator::NEQ: return "!=";
    case BinaryOperator::LT: return "<";
    case BinaryOperator::LEQ: return "<=";
    case BinaryOperator::GT: return ">";
    case BinaryOperator::GEQ: return ">=";
    case BinaryOperator::ADD: return "+";
    case BinaryOperator::SUB: return "-";
    case BinaryOperator::CONCAT: return "++";
    case BinaryOperator::DIFF: return "--";
    case BinaryOperator::MUL: return "*";
    case BinaryOperator::DIV: return "/";
    case BinaryOperator::MOD: return "%";
    case BinaryOperator::FILTER: return "?";
    case BinaryOperator::MAP: return "->";
    case BinaryOperator::ACCESS: return "[]";
    case BinaryOperator::CAST_TO: return "cast_to";
  }
}

std::string_view operatorToStr( const UnaryOperator unary_operator ) noexcept {
  switch ( unary_operator ) {
    case UnaryOperator::NEG: return "-";
    case UnaryOperator::NOT: return "not";
    case UnaryOperator::REV: return "@";
    case UnaryOperator::LEN: return "$";
  }
}

}  // namespace parser_helper