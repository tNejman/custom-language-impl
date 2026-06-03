#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <variant>
#include <vector>

#include "Drivers/Formatter.hpp"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Value.hpp"

using BuiltinFunctionActionP = std::function<std::optional<Value>( const std::vector<Value>& )>;
struct BuiltinFunction {
 private:
  std::string_view identifier_;
  Type return_type_;
  std::vector<ParameterDecl> parameters_;
  BuiltinFunctionActionP mapped_function_;

 public:
  BuiltinFunction( std::string_view identifier, Type return_type, std::vector<ParameterDecl> parameters,
                   BuiltinFunctionActionP mapped_function )
      : identifier_( identifier ),
        return_type_( std::move( return_type ) ),
        parameters_( std::move( parameters ) ),
        mapped_function_( mapped_function ) {
  }
  BuiltinFunction( BuiltinFunction&& other )
      : identifier_( other.identifier_ ),
        return_type_( std::move( other.return_type_ ) ),
        parameters_( std::move( other.parameters_ ) ),
        mapped_function_( other.mapped_function_ ) {
  }

  [[nodiscard]] std::string_view getIdentifier() const noexcept {
    return identifier_;
  }
  [[nodiscard]] const Type& getReturnType() const noexcept {
    return return_type_;
  }
  [[nodiscard]] const std::vector<ParameterDecl>& getParameters() const noexcept {
    return parameters_;
  }
  [[nodiscard]] BuiltinFunctionActionP getMappedFunction() const noexcept {
    return mapped_function_;
  }
};

namespace builtin_functions_helper {

bool isAllChars( const std::vector<Value>& values ) {
  return std::all_of( values.begin(), values.end(), []( const Value& value_in_arr ) {
    return std::holds_alternative<char>( value_in_arr.getData() );
  } );
}

bool isValueCharVector( const Value& val ) {
  return std::holds_alternative<Value::ArrayValue>( val.getData() )
         && isAllChars( std::get<Value::ArrayValue>( val.getData() ) );
}

bool isValueInt( const Value& val ) {
  return std::holds_alternative<int>( val.getData() );
}

class ExitException : public std::runtime_error {
 public:
  ExitException( int code )
      : std::runtime_error( std::format( "Explicitly called program termination. Exit code {}", code ) ) {
  }
};
};  // namespace builtin_functions_helper

namespace builtin_functions {

std::optional<Value> write( const std::vector<Value>& args ) {
  std::string combined;
  for ( const auto& val : args ) {
    combined += std::format( "{} ", val );
  }
  std::println( "{}", combined );
  return std::nullopt;
}

std::optional<Value> read( const std::vector<Value>& args ) {
  if ( args.size() != 1 || !builtin_functions_helper::isValueCharVector( args[0] ) ) {
    throw std::runtime_error( "function read() may accept (char, char, ...) or (char[]) only as prompt" );
  }
  std::string input;
  std::cin >> input;
  return Value::ArrayValue( std::vector<Value>( input.begin(), input.end() ) );
}

std::optional<Value> exit( const std::vector<Value>& args ) {
  if ( args.size() != 1 || !builtin_functions_helper::isValueInt( args[0] ) ) {
    throw std::runtime_error(
        "incompatible call with signature; note: function exit() accepts only one int (code) as arg" );
  }
  throw builtin_functions_helper::ExitException( std::get<int>( args[0].getData() ) );
}
}  // namespace builtin_functions