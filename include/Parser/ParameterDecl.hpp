#pragma once

#include <string>

#include "Parser/Types.hpp"
#include "Variable.h"

enum class PassMode { COPY, REFERENCE };

struct ParameterDecl {
  const std::string identifier_;
  const Type type_;
  const PassMode pass_mode_;
  const Mutability mutability_;

  ParameterDecl( std::string identifier, Type type, PassMode pass_mode, Mutability mutability )
      : identifier_( std::move( identifier ) ),
        type_( std::move( type ) ),
        pass_mode_( pass_mode ),
        mutability_( mutability ) {
  }

  bool operator==( const ParameterDecl& other ) const noexcept {
    return this->identifier_ == other.identifier_ && this->type_ == other.type_ && this->pass_mode_ == other.pass_mode_
           && this->mutability_ == other.mutability_;
  }
};