#pragma once

#include <variant>

#include "Parser/Node.h"
#include "Parser/Types.hpp"

namespace interpreter_helper {
template <class... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
Overloaded( Ts... ) -> Overloaded<Ts...>;

bool isLogicalOp( BinaryOperator op ) noexcept {
  switch ( op ) {
    case BinaryOperator::OR:
    case BinaryOperator::AND: return true;
    default: return false;
  }
}
bool isEqualityOp( BinaryOperator op ) noexcept {
  switch ( op ) {
    case BinaryOperator::EQ:
    case BinaryOperator::NEQ: return true;
    default: return false;
  }
}
bool isRelationalOp( BinaryOperator op ) noexcept {
  switch ( op ) {
    case BinaryOperator::LT:
    case BinaryOperator::LEQ:
    case BinaryOperator::GT:
    case BinaryOperator::GEQ: return true;
    default: return false;
  }
}
bool isNumericOp( BinaryOperator op ) noexcept {
  switch ( op ) {
    case BinaryOperator::ADD:
    case BinaryOperator::SUB:
    case BinaryOperator::MUL:
    case BinaryOperator::DIV:
    case BinaryOperator::MOD: return true;
    default: return false;
  }
}
bool isSetOp( BinaryOperator op ) noexcept {
  switch ( op ) {
    case BinaryOperator::CONCAT:
    case BinaryOperator::DIFF: return true;
    default: return false;
  }
}

bool canApplyLogicalOp( const Type& type ) noexcept {
  return type == BaseType::BOOL;
}
bool canApplyRelationalOp( const Type& type ) noexcept {
  return type == BaseType::INT || type == BaseType::FLOAT || type == BaseType::CHAR;
}
bool canApplyNumericOp( const Type& type ) noexcept {
  return type == BaseType::INT || type == BaseType::FLOAT;
}
bool canApplySetOp( const Type& type ) noexcept {
  return std::holds_alternative<ArrayType>( type.internal_ ) || type == BaseType::CHAR;
}

}  // namespace interpreter_helper