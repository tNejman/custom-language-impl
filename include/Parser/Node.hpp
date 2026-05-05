#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Lexer/Token.hpp"
#include "Parser/ParameterDecl.h"
#include "Parser/Types.hpp"
#include "Parser/Value.h"
#include "Types.hpp"
#include "Variable.h"

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

using Block = std::vector<std::unique_ptr<INode>>;

class FunctionDefNode : public INode {
 private:
  const Position declaration_position_;
  const std::string identifier_;
  const Type return_type_;
  const std::vector<std::unique_ptr<ParameterDecl>> parameters_;
  const Block body_;

 public:
  FunctionDefNode( const Position decl_position, std::string identifier, Type ret_type,
                   std::vector<std::unique_ptr<ParameterDecl>> parameters, Block body );
};

class IExpressionNode : public INode {};

class VarOrConstDeclNode : public INode {
 private:
  const Position position_;
  const std::string identifier_;
  const Mutability mutability_;
  const Type variable_decl_type_;

  std::unique_ptr<IExpressionNode> initializer_expression_;

 public:
  VarOrConstDeclNode( Position positon, std::string identifier, Mutability mutability, Type type,
                      std::unique_ptr<IExpressionNode> initializer_expression );
};

class BlockNode : public INode {
 private:
  std::vector<std::unique_ptr<INode>> statements_;
};

class IfStatementNode : public INode {
 private:
  std::vector<std::pair<std::unique_ptr<IExpressionNode>, Block>> if_then_pairs_;
  Block else_body_;

 public:
  IfStatementNode( std::vector<std::pair<std::unique_ptr<IExpressionNode>, Block>> if_then_pairs, Block else_body );
};

class WhileStatementNode : public INode {
 private:
  std::unique_ptr<IExpressionNode> condition_;
  Block body_;

 public:
  WhileStatementNode( std::unique_ptr<IExpressionNode> condition, Block body );
};

enum class ControlFlowType { CONTINUE, BREAK };
class ControlFlowNode : public INode {
 private:
  const ControlFlowType type_;

 public:
  ControlFlowNode( ControlFlowType type );
};

class ReturnNode : public INode {
 private:
  std::unique_ptr<IExpressionNode> expression_;

 public:
  ReturnNode( std::unique_ptr<IExpressionNode> expression );
};

enum class AssignmentType { ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN };

class AssignmentExprNode : public IExpressionNode {
 private:
  std::unique_ptr<IExpressionNode> left_operand_;
  std::unique_ptr<IExpressionNode> right_operand_;
  AssignmentType assign_op_;

 public:
  AssignmentExprNode( std::unique_ptr<IExpressionNode> left_operand, std::unique_ptr<IExpressionNode> right_operand,
                      AssignmentType type );
};

enum class BinaryOperator {
  OR,
  AND,
  EQ,
  NEQ,
  LT,
  LEQ,
  GT,
  GEQ,
  ADD,
  SUB,
  CONCAT,
  DIFF,
  MUL,
  DIV,
  MOD,
  FILTER,
  MAP,
  ACCESS
};
class BinaryExprNode : public IExpressionNode {  // logical or and, equality, relational, additive, multiplicative
 private:
  std::unique_ptr<IExpressionNode> left_operand_;
  std::unique_ptr<IExpressionNode> right_operand_;
  BinaryOperator operator_;

 public:
  BinaryExprNode( std::unique_ptr<IExpressionNode> left_operand, std::unique_ptr<IExpressionNode> right_operand,
                  BinaryOperator op );
};

enum class UnaryOperator { NEG, NOT, REV, LEN };

class UnaryExprNode : public IExpressionNode {  // -, not, @, $
 private:
  std::unique_ptr<IExpressionNode> operand_;
  UnaryOperator operator_;

 public:
  UnaryExprNode( std::unique_ptr<IExpressionNode> opernad, UnaryOperator un_operator );
};

class CallOrExprNode : public IExpressionNode {
 private:
  // identifier
  // argument list
};

class LiteralExprNode : public IExpressionNode {
 private:
  Type type_;
  Value value_;

 public:
  LiteralExprNode( Type type, Value value );
};

class PrimaryIdentifierNode : public IExpressionNode {
 private:
  const std::string name_;

 public:
  PrimaryIdentifierNode( std::string name );
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