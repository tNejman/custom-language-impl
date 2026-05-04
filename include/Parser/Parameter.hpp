#pragma once

#include <string>

#include "Parser/Types.hpp"


struct Parameter {
  std::string name_;
  Type type_;

  Parameter( std::string name, Type type ) : name_( name ), type_( std::move( type ) ) {
  }
};