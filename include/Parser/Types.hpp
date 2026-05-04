#pragma once

#include <memory>
#include <variant>

struct Type;

enum class BaseType { INT, FLOAT, CHAR, BOOL, VOID };

struct ArrayType {
  std::unique_ptr<Type> element_type_;
};

struct Type {
  std::variant<BaseType, ArrayType> internal_;

  Type( BaseType base_type ) : internal_( base_type ) {};
  Type( ArrayType array_type ) : internal_( std::move( array_type ) ) {};
};