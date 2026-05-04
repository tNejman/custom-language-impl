#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Lexer/Token.hpp"
#include "Parser/Parameter.hpp"
#include "Parser/Types.hpp"

class INode {
 public:
};

// class ZeroChildrenNode : public INode {
//  private:
//   // no children
// };

// class SingleChildNode : public INode {
//  private:
//   std::unique_ptr<INode> child_;
// };

// class TwoChildrenNode : public INode {
//  private:
//   std::unique_ptr<INode> child_left_;
//   std::unique_ptr<INode> child_right_;
// };

// class ManyChildrenNode : public INode {
//  private:
//   std::vector<std::unique_ptr<INode>> children_;
// };

/**
 * Concrete children
 */

class BlockNode;

class FunctionDefNode : public INode {
 private:
  Position declaration_position_;
  const std::string identifier_;
  std::unique_ptr<Type> return_type_;
  std::vector<Parameter> parameters_;
  std::unique_ptr<BlockNode> body_;

 public:
  FunctionDefNode( const Position decl_position, std::string identifier, std::unique_ptr<Type> ret_type,
                   std::vector<Parameter> parameters, std::unique_ptr<INode> body );
};

class VariableDeclNode : public INode {
 private:
  Position declaration_position_;
  bool is_mutable_;
  // Type variable_decl_type_
  // Value
 public:
};

class BlockNode : public INode {
 private:
  std::vector<std::unique_ptr<INode>> statements_;
};

class IfNode : public INode {
 private:
  // std::vector< Condition > conditions;
  // std::vector< BlockNode > bodies_;
  // ALBO
  // Condition if_cond_;
  // std::unique_ptr<BlockNode> if_body_;
  // std::vector< std::pair<Condition, std::unique_ptr<BlockNode>> > elseif_conds_;
  // std::unique_ptr<BlockNode> else_body_;
};

class WhileNode : public INode {
 private:
  // Condition while_cond_;
  std::unique_ptr<BlockNode> body_;
};

// ControlFlowNode (może)

class BinaryExprNode : public INode {  // assignment, logical or and, equality, relational, additive, multiplicative
 private:
  // std::unique_ptr<> left_operand_;
  // std::unique_ptr<> right_operand_;
  // operator
};

class UnaryExprNode : public INode {  // -, not, @, $
 private:
  // std::unique_ptr<> operand_;
  // operator
};

class CallOrExprNode : public INode {
 private:
  // identifier
  // argument list
};

class Program {
 private:
  std::vector<std::unique_ptr<INode>> statement_list_;

 public:
  Program() {
  }
  Program( std::vector<std::unique_ptr<INode>>&& statements ) : statement_list_( std::move( statements ) ) {
  }
};