#include "Interpreter/Variable.h"

#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

Variable::Variable( std::string identifier, Type type, Mutability mutability, Value value ) noexcept
    : identifier_( std::move( identifier ) ),
      type_( std::move( type ) ),
      mutability_( mutability ),
      value_( std::move( value ) ) {
}
Variable::Variable( Variable&& other ) noexcept
    : identifier_( std::move( other.identifier_ ) ),
      type_( std::move( other.type_ ) ),
      mutability_( other.mutability_ ),
      value_( std::move( other.value_ ) ) {
}

[[nodiscard]] const std::string& Variable::getIdentifier() const noexcept {
  return this->identifier_;
}
[[nodiscard]] const Type& Variable::getType() const noexcept {
  return this->type_;
}
[[nodiscard]] Mutability Variable::getMutability() const noexcept {
  return this->mutability_;
}
[[nodiscard]] const Value& Variable::getValue() const noexcept {
  return this->value_;
}
void Variable::setValue( Value new_val ) noexcept {
  this->value_ = std::move( new_val );
}

[[nodiscard]] bool Variable::operator==( const Variable& other ) const noexcept {
  return this->identifier_ == other.identifier_ && this->type_ == other.type_ && this->mutability_ == other.mutability_
         && this->value_ == other.value_;
}