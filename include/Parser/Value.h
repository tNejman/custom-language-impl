#pragma once

#include <variant>
#include <vector>

struct Value {
  using ArrayValue = std::vector<Value>;
  using VariantType = std::variant<int, float, char, bool, ArrayValue>;

 private:
  VariantType data_;

 public:
  Value( int val );
  Value( float val );
  Value( char val );
  Value( bool val );
  Value( ArrayValue val );
};