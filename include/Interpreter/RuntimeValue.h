#pragma once

#include <functional>
#include <optional>
#include <variant>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
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
  explicit RuntimeValue( Value val ) noexcept;
  explicit RuntimeValue( Variable& var ) noexcept;
  explicit RuntimeValue( std::shared_ptr<Value> val, Mutability mut ) noexcept;
  explicit RuntimeValue() noexcept;
  explicit RuntimeValue( const RuntimeValue& ) = delete;
  RuntimeValue( RuntimeValue&& other ) noexcept;

  RuntimeValue& operator=( const RuntimeValue& ) = delete;
  RuntimeValue& operator=( RuntimeValue&& other ) noexcept;
  explicit operator bool() const noexcept;

  Mutability getMutability() const noexcept;
  const std::variant<RValue, LValue, IndexRef, Void>& peekData() const noexcept;
  // std::variant<RValue, LValue, IndexRef, Void> getData() noexcept;
  bool isAssignableTo() const noexcept;
  Value copyValue() const noexcept;
  Value extractValue();
  Type getType() const noexcept;
  bool isVoid() const noexcept;

  template <typename TypeExtracted>
  TypeExtracted evaluateValue() const {
    return std::visit(
        Overloaded{
            []( const RValue& val ) -> TypeExtracted { return std::get<TypeExtracted>( val.getData() ); },
            []( const LValue& var ) -> TypeExtracted {
              return std::get<TypeExtracted>( ( *( var.get().getValue() ) ).getData() );
            },
            []( const IndexRef& i_ref ) -> TypeExtracted { return std::get<TypeExtracted>( i_ref->getData() ); },
            []( const Void& ) -> TypeExtracted {
              throw VoidValueException( Position{ 1, 1 }, "tried extracting void value" );
            } },
        data_ );
  }
};