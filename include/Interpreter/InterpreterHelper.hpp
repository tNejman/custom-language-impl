#pragma once

#include <functional>
#include <variant>

#include "Interpreter/RuntimeValue.hpp"
#include "Interpreter/Variable.h"
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

using RValue = Value;
using LValue = std::reference_wrapper<Variable>;
using AccumulatorVal = std::variant<LValue, RValue>;

// Type getAccValType( const AccumulatorVal& acc_val ) {
//   return std::visit(
//       Overloaded{ []( const Value& val ) { return val.getValueType(); },
//                   []( const std::reference_wrapper<Variable>& var ) { return var.get().getType().copy(); } },
//       acc_val );
// }

// template <typename TypeExtracted>
// TypeExtracted evaluateAccVal( const AccumulatorVal& val ) {
//   return std::visit( Overloaded{ [&]( const std::reference_wrapper<Variable> var ) {
//                                   return std::get<TypeExtracted>( var.get().getValue().getData() );
//                                 },
//                                  [&]( const Value& val ) { return std::get<TypeExtracted>( val.getData() ); } },
//                      val );
// }
// Value extractAccVal( const AccumulatorVal& val ) {
//   return std::visit(
//       Overloaded{ []( const std::reference_wrapper<Variable> var ) { return var.get().getValue().copy(); },
//                   []( const Value& val ) { return val.copy(); } },
//       val );
// }

Value extractRuntimeValue( RuntimeValue& val ) {
  return std::visit( Overloaded{ []( RValue val ) -> Value { return val; },
                                 []( LValue var ) -> Value { return var.get().getValue()->copy(); },
                                 []( IndexRef i_ref ) -> Value { return i_ref.get()->copy(); },
                                 []( Void ) -> Value { throw std::runtime_error( "tried extracting void value" ); } },
                     val.getData() );
}
Type getRuntimeValueType( const RuntimeValue& val ) {
  return std::visit(
      Overloaded{ []( const RValue& val ) -> Type { return val.getValueType().copy(); },
                  []( const LValue& val ) -> Type { return val.get().getType().copy(); },
                  []( const IndexRef& val ) -> Type { return val.get()->getValueType().copy(); },
                  []( Void ) -> Type { throw std::runtime_error( "tried getting type of void value" ); } },
      val.getData() );
}

template <typename TypeExtracted>
TypeExtracted evaluateRuntimeValue( const RuntimeValue& val ) {
  return std::visit(
      Overloaded{ []( RValue val ) -> TypeExtracted { return std::get<TypeExtracted>( val.getData() ); },
                  []( LValue var ) -> TypeExtracted { return std::get<TypeExtracted>( var.get().getType() ); },
                  []( IndexRef i_ref ) -> TypeExtracted { return std::get<TypeExtracted>( *i_ref ); },
                  []( Void ) -> TypeExtracted { throw std::runtime_error( "tried extracting void value" ); } },
      val );
}

bool isVoidFunction( const FunctionDefNode& func ) {
  return std::holds_alternative<BaseType>( func.getType().internal_ )
         && std::get<BaseType>( func.getType().internal_ ) == BaseType::VOID;
}

}  // namespace interpreter_helper