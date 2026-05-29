#pragma once

#include <string>

#include "Parser/Types.hpp"

enum class PassMode { COPY, REFERENCE };
struct ParameterDecl {
 private:
  std::string identifier_;
  Type type_;
  PassMode pass_mode_;
  Mutability mutability_;

 public:
  ParameterDecl( std::string identifier, Type type, PassMode pass_mode, Mutability mutability )
      : identifier_( std::move( identifier ) ),
        type_( std::move( type ) ),
        pass_mode_( pass_mode ),
        mutability_( mutability ) {
  }

  ParameterDecl( ParameterDecl&& other )
      : identifier_( std::move( other.identifier_ ) ),
        type_( std::move( other.type_ ) ),
        pass_mode_( other.pass_mode_ ),
        mutability_( other.mutability_ ) {
  }

  bool operator==( const ParameterDecl& other ) const noexcept {
    return this->identifier_ == other.identifier_ && this->type_ == other.type_ && this->pass_mode_ == other.pass_mode_
           && this->mutability_ == other.mutability_;
  }

  ParameterDecl& operator=( ParameterDecl&& other ) {
    if ( *this != other ) {
      this->identifier_ = std::move( other.identifier_ );
      this->type_ = std::move( other.type_ );
      this->pass_mode_ = other.pass_mode_;
      this->mutability_ = other.mutability_;
    }
    return *this;
  }

  [[nodiscard]] const std::string& getIdentifier() const noexcept {
    return identifier_;
  }
  [[nodiscard]] const Type& getType() const noexcept {
    return type_;
  }
  [[nodiscard]] const PassMode getPassMode() const noexcept {
    return pass_mode_;
  }
  [[nodiscard]] const Mutability getMutability() const noexcept {
    return mutability_;
  }
};