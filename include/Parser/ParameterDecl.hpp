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
  ParameterDecl( std::string identifier, Type type, PassMode pass_mode, Mutability mutability ) noexcept
      : identifier_( std::move( identifier ) ),
        type_( std::move( type ) ),
        pass_mode_( pass_mode ),
        mutability_( mutability ) {
  }
  ParameterDecl( const ParameterDecl& other ) = delete;
  ParameterDecl( ParameterDecl&& other )
      : identifier_( std::move( other.identifier_ ) ),
        type_( std::move( other.type_ ) ),
        pass_mode_( other.pass_mode_ ),
        mutability_( other.mutability_ ) {
  }

  bool operator==( const ParameterDecl& other ) const noexcept = default;
  ParameterDecl& operator=( ParameterDecl&& other ) noexcept = default;
  ParameterDecl& operator=( const ParameterDecl& ) = delete;

  [[nodiscard]] const std::string& getIdentifier() const noexcept {
    return identifier_;
  }
  [[nodiscard]] const Type& getType() const noexcept {
    return type_;
  }
  [[nodiscard]] PassMode getPassMode() const noexcept {
    return pass_mode_;
  }
  [[nodiscard]] Mutability getMutability() const noexcept {
    return mutability_;
  }
  ParameterDecl copy() const noexcept {
    return ParameterDecl{ std::string( identifier_ ), type_.copy(), pass_mode_, mutability_ };
  }
};