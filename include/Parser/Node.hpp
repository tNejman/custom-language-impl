#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Lexer/Token.hpp"

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

class FunctionDefNode : public INode {
private:
 Position declaration_position_;
 const std::string identifier_;
//  Type return_type_
//  Parameter List parameters_
 std::unique_ptr<BlockNode> body_;
public:
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