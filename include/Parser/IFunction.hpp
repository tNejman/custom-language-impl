#pragma once

#include <string>
#include <vector>

#include "Lexer/Token.hpp"
#include "Parser/INode.hpp"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"

class IFunction : public INode {
 private:
  std::string identifier_;
  Type return_type_;
  std::vector<ParameterDecl> parameters_;

 public:
  IFunction( Position position, std::string identifier, Type return_type, std::vector<ParameterDecl> parameters )
      : INode( position ),
        identifier_( std::move( identifier ) ),
        return_type_( std::move( return_type ) ),
        parameters_( std::move( parameters ) ) {
  }
  IFunction( const IFunction& other ) = delete;
  IFunction( IFunction&& other ) noexcept = default;
  virtual ~IFunction() = default;
  IFunction& operator=( const IFunction& other ) = delete;
  IFunction& operator=( IFunction&& ) = default;
  bool operator==( const IFunction& ) const noexcept = default;

 public:
  [[nodiscard]] const std::string& getIdentifier() const noexcept {
    return identifier_;
  }
  [[nodiscard]] const Type& getReturnType() const noexcept {
    return return_type_;
  }
  [[nodiscard]] const std::vector<ParameterDecl>& getParameters() const noexcept {
    return parameters_;
  }
  [[nodiscard]] bool isVoid() const noexcept {
    return return_type_ == BaseType::VOID;
  }
  virtual void accept( Visitor& v ) const noexcept = 0;
};