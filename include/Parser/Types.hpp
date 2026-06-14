#pragma once

#include <memory>
#include <variant>

struct Type;

enum class BaseType { INT, FLOAT, CHAR, BOOL, VOID };
enum class Mutability { MUTABLE, IMMUTABLE };
enum class ArrayIdentifierOpType { FILTER, MAP };

namespace {
template <class... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
Overloaded( Ts... ) -> Overloaded<Ts...>;
}  // namespace

struct ArrayType {
  std::unique_ptr<Type> element_type_;
};

struct Type {
  std::variant<BaseType, ArrayType> internal_;

  Type( BaseType base_type ) noexcept : internal_( base_type ) {};
  Type( ArrayType array_type ) noexcept : internal_( std::move( array_type ) ) {};

  Type( Type&& other ) noexcept : internal_( std::move( other.internal_ ) ) {
  }
  Type( const Type& other ) = delete;
  Type& operator=( Type&& other ) noexcept {
    if ( this != &other ) {
      internal_ = std::move( other.internal_ );
    }
    return *this;
  };
  Type& operaotr( Type& other ) = delete;

  bool operator==( BaseType base_type ) const noexcept {
    if ( !std::holds_alternative<BaseType>( this->internal_ ) ) return false;
    return std::get<BaseType>( this->internal_ ) == base_type;
  }

  bool operator==( const Type& other ) const noexcept {
    bool is_base_this = std::holds_alternative<BaseType>( this->internal_ );
    bool is_base_other = std::holds_alternative<BaseType>( other.internal_ );

    if ( is_base_this && is_base_other ) {
      return std::get<BaseType>( this->internal_ ) == std::get<BaseType>( other.internal_ );
    } else if ( !is_base_this && !is_base_other ) {
      return *( std::get<ArrayType>( this->internal_ ).element_type_ )
             == *( std::get<ArrayType>( other.internal_ ).element_type_ );
    }
    return false;
  }

  static Type buildTypeArrayTypeFromBase( const BaseType base_type ) noexcept {
    return Type{ ArrayType{ std::make_unique<Type>( base_type ) } };
  }

  static Type buildTypeArrayFromBaseNRec( const BaseType base_type, const unsigned int arr_rec_count ) noexcept {
    Type built_type{ base_type };
    for ( auto i = 0u; i < arr_rec_count; ++i ) {
      built_type = Type{ ArrayType{ std::make_unique<Type>( std::move( built_type ) ) } };
    }
    return built_type;
  }

  Type copy() const noexcept {
    return std::visit(
        Overloaded{ [&]( BaseType base_type ) { return Type{ base_type }; },
                    [&]( const ArrayType& arr_type ) {
                      return Type{ ArrayType{ std::make_unique<Type>( arr_type.element_type_->copy() ) } };
                    } },
        internal_ );
  }
};