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

class Visitor;

class INode {
 private:
  const Position position_;

 public:
  INode( Position position ) : position_( position ) {
  }
  virtual ~INode() = default;
  virtual void accept( Visitor& v ) = 0;
  Position getPosition() const noexcept {
    return position_;
  }
};

using Block = std::vector<std::unique_ptr<INode>>;

class FunctionDefNode : public INode {
 private:
  const std::string identifier_;
  const Type return_type_;
  const std::vector<std::unique_ptr<ParameterDecl>> parameters_;
  const Block body_;

 public:
  FunctionDefNode( Position position, std::string identifier, Type ret_type,
                   std::vector<std::unique_ptr<ParameterDecl>> parameters, Block body );
  void accept( Visitor& v ) override;
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
  void accept( Visitor& v ) override;
};

class IfStatementNode : public INode {
 private:
  const std::vector<std::pair<std::unique_ptr<const IExpressionNode>, const Block>> cond_block_pairs_;
  const Block else_block_;

 public:
  IfStatementNode( Position position, std::vector<std::pair<std::unique_ptr<IExpressionNode>, Block>> if_then_pairs,
                   Block else_body );
  void accept( Visitor& v ) override;
};

class WhileStatementNode : public INode {
 private:
  const std::unique_ptr<const IExpressionNode> condition_;
  const Block body_;

 public:
  WhileStatementNode( Position position, std::unique_ptr<IExpressionNode> condition, Block body );
  void accept( Visitor& v ) override;
};

enum class ControlFlowType { CONTINUE, BREAK };

class ControlFlowNode : public INode {
 private:
  const ControlFlowType type_;

 public:
  ControlFlowNode( Position position, ControlFlowType type );
  void accept( Visitor& v ) override;
};

class ReturnNode : public INode {
 private:
  const std::unique_ptr<const IExpressionNode> expression_;

 public:
  ReturnNode( Position position, std::unique_ptr<IExpressionNode> expression );
  void accept( Visitor& v ) override;
};

enum class AssignmentType { ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN };

class AssignmentExprNode : public IExpressionNode {
 private:
  const std::unique_ptr<const IExpressionNode> left_operand_;
  const std::unique_ptr<const IExpressionNode> right_operand_;
  const AssignmentType assign_op_;

 public:
  AssignmentExprNode( Position position, std::unique_ptr<IExpressionNode> left_operand,
                      std::unique_ptr<IExpressionNode> right_operand, AssignmentType type );
  void accept( Visitor& v ) override;
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
  const std::unique_ptr<const IExpressionNode> left_operand_;
  const std::unique_ptr<const IExpressionNode> right_operand_;
  const BinaryOperator operator_;

 public:
  BinaryExprNode( std::unique_ptr<IExpressionNode> left_operand, std::unique_ptr<IExpressionNode> right_operand,
                  BinaryOperator op );
  void accept( Visitor& v ) override;
};

enum class UnaryOperator { NEG, NOT, REV, LEN };

class UnaryExprNode : public IExpressionNode {  // -, not, @, $
 private:
  const std::unique_ptr<const IExpressionNode> operand_;
  const UnaryOperator operator_;

 public:
  UnaryExprNode( Position position, std::unique_ptr<IExpressionNode> opernad, UnaryOperator un_operator );
  void accept( Visitor& v ) override;
};

using ExpressionVec = const std::vector<std::unique_ptr<const IExpressionNode>>;

class FunctionCallNode : public IExpressionNode {
 private:
  const std::string identifier_;
  ExpressionVec arguments_;

 public:
  FunctionCallNode( Position position, std::string, std::vector<std::unique_ptr<IExpressionNode>> arguments );
  void accept( Visitor& v ) override;
};

class ArrayLiteralNode : public IExpressionNode {
 private:
  const ExpressionVec positions_;

 public:
  ArrayLiteralNode( Position position, std::vector<std::unique_ptr<IExpressionNode>> positions );
  void accept( Visitor& v ) override;
};

class LiteralExprNode : public IExpressionNode {
 private:
  const Type type_;
  const Value value_;

 public:
  LiteralExprNode( Position position, Type type, Value value );
  void accept( Visitor& v ) override;
};

class PrimaryIdentifierNode : public IExpressionNode {
 private:
  const std::string name_;

 public:
  PrimaryIdentifierNode( Position pos, std::string name );
  void accept( Visitor& v ) override;
};

class ProgramNode : public INode {
 private:
  const std::vector<std::unique_ptr<const INode>> statement_list_;

 public:
  ProgramNode( Position pos, std::vector<std::unique_ptr<INode>> statements );
  void accept( Visitor& v ) override;
};