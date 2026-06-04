#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <variant>
#include <vector>

#include "Drivers/Formatter.hpp"  // IWYU pragma: keep
#include "Interpreter/RuntimeValue.h"
#include "Parser/IFunction.hpp"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"
#include "VecCopy.hpp"

using BuiltinFunctionActionP = std::function<std::optional<Value>( const std::vector<Value>& )>;
class BuiltinFunction : public IFunction {
 private:
  BuiltinFunctionActionP mapped_function_;

 public:
  BuiltinFunction( Position position, std::string identifier, Type return_type, std::vector<ParameterDecl> parameters,
                   BuiltinFunctionActionP mapped_function )
      : IFunction( position, std::move( identifier ), std::move( return_type ), std::move( parameters ) ),
        mapped_function_( mapped_function ) {
    assert( mapped_function_ != nullptr );
  }
  BuiltinFunction( BuiltinFunction&& other )
      : IFunction( std::move( other ) ), mapped_function_( other.mapped_function_ ) {
  }
  [[nodiscard]] BuiltinFunctionActionP getMappedFunction() const noexcept {
    return mapped_function_;
  }
  void accept( Visitor& v ) const noexcept override {
    v.visit( *this );
  }
  bool operator==( const BuiltinFunction& other ) const noexcept {
    using FuncPtr = std::optional<Value> ( * )( const std::vector<Value>& );
    return static_cast<const IFunction&>( *this ) == static_cast<const IFunction&>( other )
           && *( this->mapped_function_.target<FuncPtr>() ) == *( other.mapped_function_.target<FuncPtr>() );
  }
};

namespace builtin_functions_helper {

inline bool isAllChars( const std::vector<Value>& values ) {
  return std::all_of( values.begin(), values.end(), []( const Value& value_in_arr ) {
    return std::holds_alternative<char>( value_in_arr.getData() );
  } );
}

inline bool isValueCharVector( const Value& val ) {
  return std::holds_alternative<Value::ArrayValue>( val.getData() )
         && isAllChars( std::get<Value::ArrayValue>( val.getData() ) );
}

inline bool isValueInt( const Value& val ) {
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

inline std::optional<Value> write( const std::vector<Value>& args ) {
  std::string combined;
  for ( const auto& val : args ) {
    combined += std::format( "{} ", val );
  }
  std::println( "{}", combined );
  return std::nullopt;
}

inline std::optional<Value> read( const std::vector<Value>& args ) {
  if ( args.size() != 1 || !builtin_functions_helper::isValueCharVector( args[0] ) ) {
    throw std::runtime_error( "function read() may accept (char, char, ...) or (char[]) only as prompt" );
  }
  std::string input;
  std::cin >> input;
  return Value::ArrayValue( std::vector<Value>( input.begin(), input.end() ) );
}

inline std::optional<Value> exit( const std::vector<Value>& args ) {
  if ( args.size() != 1 || !builtin_functions_helper::isValueInt( args[0] ) ) {
    throw std::runtime_error(
        "incompatible call with signature; note: function exit() accepts only one int (code) as arg" );
  }
  throw builtin_functions_helper::ExitException( std::get<int>( args[0].getData() ) );
}

inline BuiltinFunction buildBuiltinWrite( const std::vector<RuntimeValue>& call_args ) {
  std::vector<ParameterDecl> params;
  int i = 0;
  for ( const auto& arg : call_args ) {
    params.push_back(
        ParameterDecl{ std::format( "x{}", i ), arg.getType().copy(), PassMode::COPY, Mutability::IMMUTABLE } );
  }
  return BuiltinFunction{ Position{ 99999u, 99999u }, "write", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
                          std::move( params ), builtin_functions::write };
}

inline BuiltinFunction buildBuiltinWrite( const std::vector<ParameterDecl>& call_args ) {
  return BuiltinFunction{ Position{ 99999u, 99999u }, "write", Type::buildTypeArrayTypeFromBase( BaseType::CHAR ),
                          copyVec( call_args ), builtin_functions::write };
}
}  // namespace builtin_functions