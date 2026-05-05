#pragma once

#include <string>

#include "Parser/Types.hpp"
#include "Variable.h"

enum class PassMode { COPY, REFERENCE };

struct ParameterDecl {
  const std::string name_;
  const Type type_;
  const PassMode pass_mode_;
  const Mutability mutability_;

  ParameterDecl( std::string name, Type type, PassMode pass_mode, Mutability mutability );
};