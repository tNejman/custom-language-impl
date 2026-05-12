#pragma once

#include <sstream>

#include "Drivers/Formatter.hpp"
#include "Parser/Node.h"
#include "Parser/Visitor.h"

class AstSerializerVisitor : public Visitor {
 private:
  constexpr static std::string_view OPENING_SECTION_MARKER = "{";
  constexpr static std::string_view CLOSING_SECTION_MARKER = "}";
  // constexpr static std::string_view PROGRAM_OPENING_SECTION_MARKER = "<<";
  // constexpr static std::string_view PROGRAM_CLOSING_SECTION_MARKER = ">>";
  std::stringstream string_builder_;
  void flushBuffer();

  void serializeBlock( const Block& block );

 public:
  AstSerializerVisitor() {
    flushBuffer();
  }

  void visit( const FunctionDefNode& node ) override;
  // void visit( const IExpressionNode& node ) override;
  void visit( const VarOrConstDeclNode& node ) override;
  void visit( const IfStatementNode& node ) override;
  void visit( const WhileStatementNode& node ) override;
  void visit( const ControlFlowNode& node ) override;
  void visit( const ReturnNode& node ) override;
  void visit( const AssignmentExprNode& node ) override;
  void visit( const BinaryExprNode& node ) override;
  void visit( const UnaryExprNode& node ) override;
  void visit( const CastExprNode& node ) override;
  void visit( const FunctionCallNode& node ) override;
  void visit( const ArrayLiteralNode& node ) override;
  void visit( const LiteralExprNode& node ) override;
  void visit( const PrimaryIdentifierNode& node ) override;
  void visit( const ProgramNode& node ) override;

  std::string getString() noexcept;
};

class VisitGuard {
 private:
  std::stringstream& string_stream_;
  const std::string_view opening_marker_;
  const std::string_view closing_marker_;

 public:
  VisitGuard( std::stringstream& string_stream, const std::string_view opening_marker,
              const std::string_view closing_marker )
      : string_stream_( string_stream ), opening_marker_( opening_marker ), closing_marker_( closing_marker ) {
    string_stream_ << opening_marker;
  }
  ~VisitGuard() {
    string_stream_ << closing_marker_;
  }
};