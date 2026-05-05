#pragma once

#include <string>

#include "Parser/Types.hpp"
#include "Parser/Value.h"

enum class Mutability { MUTABLE, IMMUTABLE };

struct Variable {
 private:
  const std::string name_;
  const Type type_;
  const Mutability mutability_;
  Value value_;

 public:
  Variable( std::string name, Type type, Mutability mutability );

  const std::string& getName() const;
  const Type& getType() const;
  const Mutability getMutability() const;
  const Value& getValue() const;
};