#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Lexer/Token.hpp"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"
#include "Parser/Visitor.h"

class INode {
 private:
  const Position position_;

 public:
  INode( Position position ) : position_( position ) {
  }
  virtual ~INode() = default;
  virtual void accept( Visitor& v ) const noexcept = 0;
  Position getPosition() const noexcept {
    return position_;
  }
};

using Block = std::vector<std::unique_ptr<INode>>;

class FunctionDefNode : public INode {
 private:
  const std::string identifier_;
  const Type return_type_;
  const std::vector<ParameterDecl> parameters_;
  const Block block_;

 public:
  FunctionDefNode( Position position, std::string identifier, Type ret_type, std::vector<ParameterDecl> parameters,
                   Block block );
  void accept( Visitor& v ) const noexcept override;
  std::string_view getIdentifier() const noexcept;
  const Type& getType() const noexcept;
  const std::vector<ParameterDecl>& getParameters() const noexcept;
  const Block& getBlock() const noexcept;
  bool isVoid() const noexcept;
};

class IExpressionNode : public INode {
 public:
  IExpressionNode( Position position ) : INode( position ) {
  }
  ~IExpressionNode() = default;
  virtual void accept( Visitor& v ) const noexcept override = 0;
};

class VarOrConstDeclNode : public INode {
 private:
  const std::string identifier_;
  const Mutability mutability_;
  const Type variable_decl_type_;

  std::unique_ptr<IExpressionNode> initializer_expression_;

 public:
  VarOrConstDeclNode( Position positon, std::string identifier, Mutability mutability, Type type,
                      std::unique_ptr<IExpressionNode> initializer_expression );
  void accept( Visitor& v ) const noexcept override;
  std::string_view getIdentifier() const noexcept;
  Mutability getMutability() const noexcept;
  const Type& getType() const noexcept;
  const IExpressionNode* getInitializerExpr() const noexcept;
};

using ExprBlockPairVec = std::vector<std::pair<std::unique_ptr<IExpressionNode>, Block>>;

class IfStatementNode : public INode {
 private:
  const ExprBlockPairVec cond_block_pairs_;
  const Block else_block_;

 public:
  IfStatementNode( Position position, ExprBlockPairVec if_then_pairs, Block else_block );
  void accept( Visitor& v ) const noexcept override;
  const ExprBlockPairVec& getCondBlockPairs() const noexcept;
  const Block& getElseBlock() const noexcept;
};

class WhileStatementNode : public INode {
 private:
  const std::unique_ptr<const IExpressionNode> condition_;
  const Block block_;

 public:
  WhileStatementNode( Position position, std::unique_ptr<IExpressionNode> condition, Block block );
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode* getCondition() const noexcept;
  const Block& getBlock() const noexcept;
};

enum class ControlFlowType { CONTINUE, BREAK };

class ControlFlowNode : public INode {
 private:
  const ControlFlowType type_;

 public:
  ControlFlowNode( Position position, ControlFlowType type );
  void accept( Visitor& v ) const noexcept override;
  ControlFlowType getControlFlowType() const noexcept;
};

class ReturnNode : public INode {
 private:
  const std::unique_ptr<const IExpressionNode> expression_;

 public:
  ReturnNode( Position position, std::unique_ptr<IExpressionNode> expression );
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode* getExpression() const noexcept;
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
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode* getLeftOperand() const noexcept;
  const IExpressionNode* getRightOperand() const noexcept;
  AssignmentType getAssignmentType() const noexcept;
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
  ACCESS,
};

class BinaryExprNode : public IExpressionNode {  // logical or and, equality, relational, additive, multiplicative
 private:
  const std::unique_ptr<const IExpressionNode> left_operand_;
  const std::unique_ptr<const IExpressionNode> right_operand_;
  const BinaryOperator operator_;

 public:
  BinaryExprNode( Position position, std::unique_ptr<IExpressionNode> left_operand,
                  std::unique_ptr<IExpressionNode> right_operand, BinaryOperator op );
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode* getLeftOperand() const noexcept;
  const IExpressionNode* getRightOperand() const noexcept;
  BinaryOperator getOperator() const noexcept;
};

enum class UnaryOperator { NEG, NOT, REV, LEN };

class UnaryExprNode : public IExpressionNode {  // -, not, @, $
 private:
  const std::unique_ptr<const IExpressionNode> operand_;
  const UnaryOperator operator_;

 public:
  UnaryExprNode( Position position, std::unique_ptr<IExpressionNode> opernad, UnaryOperator un_operator );
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode* getOperand() const noexcept;
  UnaryOperator getOperator() const noexcept;
};

class CastExprNode : public IExpressionNode {  // cast to
 private:
  const std::unique_ptr<const IExpressionNode> expression_;
  const Type type_;

 public:
  CastExprNode( Position position, std::unique_ptr<IExpressionNode> expression, Type type );
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode* getExpression() const noexcept;
  const Type& getType() const noexcept;
};

class ArrayIdentifierOpNode : public IExpressionNode {
 private:
  const std::unique_ptr<const IExpressionNode> left_operand_;
  const ArrayIdentifierOpType type_;
  const std::string identifier_;

 public:
  ArrayIdentifierOpNode( Position position, std::unique_ptr<IExpressionNode> left_operand, ArrayIdentifierOpType type,
                         std::string identifier );
  void accept( Visitor& v ) const noexcept override;
  const IExpressionNode& getExpression() const noexcept;
  ArrayIdentifierOpType getType() const noexcept;
  const std::string& getIdentifier() const noexcept;
};

using ExpressionVec = std::vector<std::unique_ptr<IExpressionNode>>;

class FunctionCallNode : public IExpressionNode {
 private:
  const std::string identifier_;
  const ExpressionVec arguments_;

 public:
  FunctionCallNode( Position position, std::string identifier, ExpressionVec arguments );
  void accept( Visitor& v ) const noexcept override;
  std::string_view getIdentifier() const noexcept;
  const ExpressionVec& getArguments() const noexcept;
};

class ArrayLiteralNode : public IExpressionNode {
 private:
  const ExpressionVec array_positions_;

 public:
  ArrayLiteralNode( Position position, ExpressionVec array_positions );
  void accept( Visitor& v ) const noexcept override;
  const ExpressionVec& getPositions() const noexcept;
};

class LiteralExprNode : public IExpressionNode {
 private:
  const Type type_;
  const Value value_;

 public:
  LiteralExprNode( Position position, Type type, Value value );
  void accept( Visitor& v ) const noexcept override;
  const Type& getType() const noexcept;
  const Value& getValue() const noexcept;
};

class PrimaryIdentifierNode : public IExpressionNode {
 private:
  const std::string identifier_;

 public:
  PrimaryIdentifierNode( Position position, std::string identifier );
  void accept( Visitor& v ) const noexcept override;
  std::string_view getIdentifier() const noexcept;
};

class ProgramNode : public INode {
 private:
  const std::vector<std::unique_ptr<INode>> statement_list_;
  const std::vector<std::unique_ptr<FunctionDefNode>> function_definitions_;

 public:
  ProgramNode( Position position, std::vector<std::unique_ptr<INode>> statements,
               std::vector<std::unique_ptr<FunctionDefNode>> functions );
  void accept( Visitor& v ) const noexcept override;
  const std::vector<std::unique_ptr<INode>>& getStatementList() const noexcept;
  const std::vector<std::unique_ptr<FunctionDefNode>>& getFunctionList() const noexcept;
};

struct NextParsedNode {
  std::unique_ptr<INode> statement_;
  std::unique_ptr<FunctionDefNode> function_def_;

  explicit operator bool() const {
    return statement_ != nullptr || function_def_ != nullptr;
  }
};