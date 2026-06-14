#pragma once

class FunctionDefNode;
class IExpressionNode;
class VarOrConstDeclNode;
class IfStatementNode;
class WhileStatementNode;
class ControlFlowNode;
class ReturnNode;
class AssignmentExprNode;
class BinaryExprNode;
class UnaryExprNode;
class CastExprNode;
class ArrayIdentifierOpNode;
class FunctionCallNode;
class ArrayLiteralNode;
class LiteralExprNode;
class PrimaryIdentifierNode;
class ProgramNode;

class BuiltinFunction;

class Visitor {
 public:
  virtual ~Visitor() = default;

  virtual void visit( const FunctionDefNode& node ) = 0;
  virtual void visit( const VarOrConstDeclNode& node ) = 0;
  virtual void visit( const IfStatementNode& node ) = 0;
  virtual void visit( const WhileStatementNode& node ) = 0;
  virtual void visit( const ControlFlowNode& node ) = 0;
  virtual void visit( const ReturnNode& node ) = 0;
  virtual void visit( const AssignmentExprNode& node ) = 0;
  virtual void visit( const BinaryExprNode& node ) = 0;
  virtual void visit( const UnaryExprNode& node ) = 0;
  virtual void visit( const CastExprNode& node ) = 0;
  virtual void visit( const ArrayIdentifierOpNode& node ) = 0;
  virtual void visit( const FunctionCallNode& node ) = 0;
  virtual void visit( const ArrayLiteralNode& node ) = 0;
  virtual void visit( const LiteralExprNode& node ) = 0;
  virtual void visit( const PrimaryIdentifierNode& node ) = 0;
  virtual void visit( const ProgramNode& node ) = 0;

  virtual void visit( const BuiltinFunction& node ) = 0;
};