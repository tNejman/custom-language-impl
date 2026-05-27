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
    for ( int i = 0; i < arr_rec_count; ++i ) {
      built_type = Type{ ArrayType{ std::make_unique<Type>( std::move( built_type ) ) } };
    }
    return built_type;
  }
};