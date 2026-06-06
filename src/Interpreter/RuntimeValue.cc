#include "Interpreter/RuntimeValue.h"

#include <variant>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"

RuntimeValue::RuntimeValue( Value val ) noexcept : mutability_( Mutability::IMMUTABLE ), data_( std::move( val ) ) {
}
RuntimeValue::RuntimeValue( Variable& var ) noexcept : mutability_( var.getMutability() ), data_( var ) {};
RuntimeValue::RuntimeValue( std::shared_ptr<Value> val, Mutability mut ) noexcept
    : mutability_( mut ), data_( std::ref( val ) ) {
}
RuntimeValue::RuntimeValue() noexcept : mutability_( Mutability::IMMUTABLE ), data_( Void{} ) {
}
RuntimeValue::RuntimeValue( RuntimeValue&& other ) noexcept
    : mutability_( other.mutability_ ), data_( std::move( other.data_ ) ) {
}
RuntimeValue& RuntimeValue::operator=( RuntimeValue&& other ) noexcept {
  this->mutability_ = other.mutability_;
  this->data_ = std::move( other.data_ );
  return *this;
}
RuntimeValue::operator bool() const noexcept {
  return isVoid();
}
Mutability RuntimeValue::getMutability() const noexcept {
  return mutability_;
}
const std::variant<RValue, LValue, IndexRef, Void>& RuntimeValue::peekData() const noexcept {
  return data_;
}
// std::variant<RValue, LValue, IndexRef, Void> RuntimeValue::getData() noexcept {
//   return std::exchange( data_, Void{} );
// }
bool RuntimeValue::isAssignableTo() const noexcept {
  return std::visit( Overloaded{ []( const RValue& _ ) { return false; },
                                 [&]( const LValue& _ ) { return mutability_ == Mutability::MUTABLE; },
                                 [&]( const IndexRef& _ ) { return mutability_ == Mutability::MUTABLE; },
                                 []( const Void& _ ) { return false; } },
                     data_ );
}

Value RuntimeValue::extractValue() {
  return std::visit( Overloaded{ []( RValue val ) -> Value { return val; },
                                 []( LValue var ) -> Value { return var.get().getValue()->copy(); },
                                 []( IndexRef i_ref ) -> Value { return i_ref.get()->copy(); },
                                 []( Void ) -> Value { std::unreachable(); } },
                     std::move( data_ ) );
}
Type RuntimeValue::getType() const noexcept {
  return std::visit( Overloaded{ []( const RValue& val ) -> Type { return val.getValueType().copy(); },
                                 []( const LValue& val ) -> Type { return val.get().getType().copy(); },
                                 []( const IndexRef& val ) -> Type { return val.get()->getValueType().copy(); },
                                 []( Void ) -> Type { std::unreachable(); } },
                     data_ );
}
bool RuntimeValue::isVoid() const noexcept {
  return std::holds_alternative<Void>( data_ );
}