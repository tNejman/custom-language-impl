#include "Parser/Variable.h"

#include "Parser/Types.hpp"

Variable::Variable( std::string name, Type type, Mutability mutability, Value value )
    : name_( std::move( name ) ), type_( std::move( type ) ), mutability_( mutability ), value_( std::move( value ) ) {
}

const std::string& Variable::getName() const {
  return this->name_;
}

const Type& Variable::getType() const {
  return this->type_;
}

const Mutability Variable::getMutability() const {
  return this->mutability_;
}
