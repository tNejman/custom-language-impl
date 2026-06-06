#pragma once

#include "Lexer/Token.hpp"
#include "Parser/Visitor.h"

class INode {
 private:
  Position position_;

 public:
  INode( Position position ) : position_( position ) {
  }
  INode( const INode& other ) = delete;
  INode( INode&& other ) noexcept = default;
  virtual ~INode() = default;
  INode& operator=( const INode& ) = delete;
  INode& operator=( INode&& ) noexcept = default;
  bool operator==( const INode& ) const noexcept = default;
  virtual void accept( Visitor& v ) const = 0;
  Position getPosition() const noexcept {
    return position_;
  }
};