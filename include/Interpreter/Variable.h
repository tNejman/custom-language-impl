#pragma once

#include <string>

#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

struct Variable {
 private:
  std::string identifier_;
  Type type_;
  Mutability mutability_;
  Value value_;

 public:
  Variable( std::string identifier, Type type, Mutability mutability, Value value ) noexcept;
  Variable( const Variable& ) = delete;
  Variable( Variable&& other ) noexcept;

  [[nodiscard]] const std::string& getIdentifier() const noexcept;
  [[nodiscard]] const Type& getType() const noexcept;
  [[nodiscard]] const Mutability getMutability() const noexcept;
  [[nodiscard]] const Value& getValue() const noexcept;
};