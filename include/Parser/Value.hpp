#pragma once

#include <variant>
#include <vector>

struct Value {
  using ArrayValue = std::vector<Value>;
  using VariantType = std::variant<int, float, char, bool, ArrayValue>;

 private:
  VariantType data_;

 public:
  Value( int val ) : data_( val ) {
  }
  Value( float val ) : data_( val ) {
  }
  Value( char val ) : data_( val ) {
  }
  Value( bool val ) : data_( val ) {
  }
  Value( ArrayValue val ) : data_( val ) {
  }

  const VariantType& getData() const noexcept {
    return data_;
  }
};