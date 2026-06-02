#pragma once

#include <functional>
#include <optional>
#include <variant>

#include "Interpreter/Variable.h"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

using RValue = Value;
using LValue = std::reference_wrapper<Variable>;
using IndexRef = std::shared_ptr<Value>;
using Void = std::monostate;

struct RuntimeValue {
 private:
  Mutability mutability_;
  std::variant<RValue, LValue, IndexRef, Void> data_;

 public:
  explicit RuntimeValue( Value val ) noexcept : mutability_( Mutability::IMMUTABLE ), data_( std::move( val ) ) {
  }
  explicit RuntimeValue( Variable& var ) noexcept : mutability_( var.getMutability() ), data_( var ) {};
  explicit RuntimeValue( std::shared_ptr<Value> val, Mutability mut ) noexcept : mutability_( mut ), data_( std::ref( val ) ) {
  }
  explicit RuntimeValue() noexcept : mutability_( Mutability::IMMUTABLE ), data_( Void{} ) {
  }
  explicit RuntimeValue( const RuntimeValue& ) = delete;
  RuntimeValue( RuntimeValue&& other ) noexcept : mutability_( other.mutability_ ), data_( std::move( other.data_ ) ) {
  }
  RuntimeValue& operator=( const RuntimeValue& ) = delete;
  RuntimeValue& operator=( RuntimeValue&& other ) noexcept {
    this->mutability_ = other.mutability_;
    this->data_ = std::move( other.data_ );
    return *this;
  }

  Mutability getMutability() const noexcept {
    return mutability_;
  }
  const std::variant<RValue, LValue, IndexRef, Void>& getData() const noexcept {
    return data_;
  }
  std::variant<RValue, LValue, IndexRef, Void> getData() noexcept {
    return std::exchange( data_, Void{} );
  }
  bool isAssignableTo() const noexcept {
    return std::visit( Overloaded{ []( const RValue& _ ) { return false; },
                                   [&]( const LValue& _ ) { return mutability_ == Mutability::MUTABLE; },
                                   [&]( const IndexRef& _ ) { return mutability_ == Mutability::MUTABLE; },
                                   []( const Void& _ ) { return false; } },
                       data_ );
  }
  explicit operator bool() const noexcept {
    return std::holds_alternative<Void>( data_ );
  }
};