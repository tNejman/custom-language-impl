#pragma once

#include <cassert>
#include <type_traits>
#include <variant>
#include <vector>

#include "Parser/Types.hpp"

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
  Value( ArrayValue val ) : data_( std::move( val ) ) {
    assert( !val.empty() && "ArrayValue must be initialized with a non-empty vector" );
  }

  Value( const Value& ) = delete;
  Value( Value&& other ) : data_( std::move( other.data_ ) ) {
  }

  Value& operator=( Value&& other ) noexcept {
    if ( *this != other ) {
      this->data_ = std::move( other.data_ );
    }
    return *this;
  }

  bool operator==( const Value& other ) const noexcept = default;
  bool operator!=( const Value& other ) const noexcept = default;

  const VariantType& getData() const noexcept {
    return data_;
  }

  Type getValueType() const noexcept {
    return std::visit( Overloaded{ []( const ArrayValue& data ) -> Type {
                                    return ArrayType{ std::make_unique<Type>( data[0].getValueType() ) };
                                  },
                                   []( const int ) -> Type { return BaseType::INT; },
                                   []( const float ) -> Type { return BaseType::FLOAT; },
                                   []( const char ) -> Type { return BaseType::CHAR; },
                                   []( const bool ) -> Type { return BaseType::BOOL; } },
                       this->data_ );
  }

  Value copy() const noexcept {
    return std::visit( Overloaded{ []( const ArrayValue& val ) -> Value {
                                    ArrayValue new_val;
                                    for ( const auto& inside_val : val ) {
                                      new_val.push_back( inside_val.copy() );
                                    }
                                    return Value{ std::move( new_val ) };
                                  },
                                   []<typename T>
                                   requires( !std::is_same_v<T, ArrayValue> )
                                   ( const T& val ) { return Value{ val }; } },
                                   this->data_ );
  }

  template <typename... Args>
  static Value makeArray( Args&&... args ) {
    Value::ArrayValue arr;
    arr.reserve( sizeof...( args ) );
    ( arr.push_back( std::move( args ) ), ... );
    return arr;
  }
};