#pragma once

#include "Parser/Node.h"
#include "Parser/Visitor.h"

class AstPrinterVisitor : public Visitor {
 private:
  int indent_level_ = 0;
  std::vector<bool> is_last_child_stack_;

  void printPrefix() const noexcept;
  void printIndent() const noexcept;

 public:
  void visit( const FunctionDefNode& node ) override;
  void visit( const IExpressionNode& node ) override;
  void visit( const VarOrConstDeclNode& node ) override;
  void visit( const IfStatementNode& node ) override;
  void visit( const WhileStatementNode& node ) override;
  void visit( const ControlFlowNode& node ) override;
  void visit( const ReturnNode& node ) override;
  void visit( const AssignmentExprNode& node ) override;
  void visit( const BinaryExprNode& node ) override;
  void visit( const UnaryExprNode& node ) override;
  void visit( const FunctionCallNode& node ) override;
  void visit( const ArrayLiteralNode& node ) override;
  void visit( const LiteralExprNode& node ) override;
  void visit( const PrimaryIdentifierNode& node ) override;
  void visit( const ProgramNode& node ) override;
};