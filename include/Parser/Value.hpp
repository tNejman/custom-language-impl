#pragma once

#include <variant>
#include <vector>

#include "Lexer/Token.hpp"

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

  // template <typename UnderlyingType>
  // requires std::is_same_v<UnderlyingType, int> || std::is_same_v<UnderlyingType, float>
  //          || std::is_same_v<UnderlyingType, char> || std::is_same_v<UnderlyingType, bool>
  // Value( TokenVal val ) : data_( std::get<UnderlyingType>( val ) ) {
  // }

  // template <typename UnderlyingType>
  // requires std::is_same_v<UnderlyingType, std::string>
  // Value( TokenVal val )
  //     : data_( std::vector<char>( std::get<std::string>( val ).begin(), std::get<std::string>( val ).end() ) ) {
  // }

  const VariantType& getData() const noexcept {
    return data_;
  }

  // template <typename TypeCompared>
  // requires std::is_same_v<TypeCompared, int> || std::is_same_v<TypeCompared, float>
  //          || std::is_same_v<TypeCompared, char> || std::is_same_v<TypeCompared, bool>
  // bool operator==( const TypeCompared val ) const noexcept {
  //   return std::holds_alternative<TypeCompared>( data_ ) && std::get<TypeCompared>( data_ ) == val;
  // }
  template <typename TypeCompared>
  bool operator==( const TypeCompared val ) const noexcept {
    return std::holds_alternative<TypeCompared>( data_ ) && std::get<TypeCompared>( data_ ) == val;
  }

  bool operator==( const Value& other ) const noexcept {
    return std::visit(
        []( const auto& val1, const auto& val2 ) -> bool {
          using T1 = std::decay_t<decltype( val1 )>;
          using T2 = std::decay_t<decltype( val2 )>;

          if constexpr ( std::is_same_v<T1, T2> ) {
            return val1 == val2;
          } else {
            return false;
          }
        },
        this->data_, other.data_ );
  }
};