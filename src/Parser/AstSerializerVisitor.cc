#include "Parser/AstSerializerVisitor.h"

#include <cmath>
#include <format>
#include <stdexcept>

#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/ParserHelper.h"

#define VIS_GUARD VisitGuard visit_guard{ string_builder_, OPENING_SECTION_MARKER, CLOSING_SECTION_MARKER };

void AstSerializerVisitor::flushBuffer() {
  string_builder_.str( "" );
  string_builder_.clear();
}

void AstSerializerVisitor::serializeBlock( const Block& block ) {
  string_builder_ << '{';
  bool is_first_stmnt = true;
  for ( const auto& stmnt_ptr : block ) {
    if ( !is_first_stmnt ) string_builder_ << "; ";
    stmnt_ptr->accept( *this );
    is_first_stmnt = false;
  }
  string_builder_ << '}';
}

void AstSerializerVisitor::visit( const FunctionDefNode& node ) {
  VIS_GUARD

  string_builder_ << std::format( "def {} {}(", node.getType(), node.getIdentifier() );
  bool is_first_param = true;
  for ( const auto& param_ptr : node.getParameters() ) {
    if ( !is_first_param ) string_builder_ << ", ";
    string_builder_ << std::format( "{}", *param_ptr );
    is_first_param = false;
  }
  string_builder_ << ") ";
  serializeBlock( node.getBlock() );
}

// void AstSerializerVisitor::visit( const IExpressionNode& node ) {
//   throw std::runtime_error( "AstSerializerVisitor tried visiting IExpressionNode, which is an interface" );
// }

void AstSerializerVisitor::visit( const VarOrConstDeclNode& node ) {
  VIS_GUARD

  const std::string_view mutability_quantifier = node.getMutability() == Mutability::MUTABLE ? "var " : "";
  string_builder_ << std::format( "{}{} {} = ", mutability_quantifier, node.getType(), node.getIdentifier() );
  node.getInitializerExpr()->accept( *this );
}

void AstSerializerVisitor::visit( const IfStatementNode& node ) {
  VIS_GUARD

  bool is_first_branch = true;
  for ( const auto& [cond_ptr, block] : node.getCondBlockPairs() ) {
    if ( is_first_branch ) {
      string_builder_ << "if (";
      is_first_branch = false;
    } else {
      string_builder_ << " elseif (";
    }
    cond_ptr->accept( *this );
    string_builder_ << ") ";

    serializeBlock( block );
  }
  string_builder_ << " else ";
  serializeBlock( node.getElseBlock() );
}

void AstSerializerVisitor::visit( const WhileStatementNode& node ) {
  VIS_GUARD

  string_builder_ << "while (";
  node.getCondition()->accept( *this );
  string_builder_ << ") ";
  serializeBlock( node.getBlock() );
}

void AstSerializerVisitor::visit( const ControlFlowNode& node ) {
  VIS_GUARD

  const std::string_view control_flow_action =
      node.getControlFlowType() == ControlFlowType::CONTINUE ? "continue" : "break";
  string_builder_ << control_flow_action;
}

void AstSerializerVisitor::visit( const ReturnNode& node ) {
  VIS_GUARD

  string_builder_ << "ret";
  if ( auto expr = node.getExpression() ) {
    string_builder_ << ' ';
    expr->accept( *this );
  }
}

void AstSerializerVisitor::visit( const AssignmentExprNode& node ) {
  VIS_GUARD

  node.getLeftOperand()->accept( *this );
  string_builder_ << ' ' << parser_helper::operatorToStr( node.getAssignmentType() ) << ' ';
  node.getRightOperand()->accept( *this );
}

void AstSerializerVisitor::visit( const BinaryExprNode& node ) {
  VIS_GUARD

  node.getLeftOperand()->accept( *this );
  std::string_view operator_str = parser_helper::operatorToStr( node.getOperator() );
  if ( operator_str != "[]" ) {
    string_builder_ << ' ' << operator_str << ' ';
    node.getRightOperand()->accept( *this );
  } else {
    string_builder_ << "[";
    node.getRightOperand()->accept( *this );
    string_builder_ << "]";
  }
}

void AstSerializerVisitor::visit( const UnaryExprNode& node ) {
  VIS_GUARD

  std::string_view operator_str = parser_helper::operatorToStr( node.getOperator() );
  if ( operator_str == "not" ) {
    string_builder_ << operator_str << ' ';
  } else {
    string_builder_ << operator_str;
  }
  node.getOperand()->accept( *this );
}

void AstSerializerVisitor::visit( const CastExprNode& node ) {
  VIS_GUARD

  node.getExpression()->accept( *this );
  string_builder_ << " cast_to " << std::format( "{}", node.getType() );
}

void AstSerializerVisitor::visit( const FunctionCallNode& node ) {
  // VIS_GUARD

  string_builder_ << std::format( "{}(", node.getIdentifier() );
  bool is_first_arg = true;
  for ( const auto& arg_ptr : node.getArguments() ) {
    if ( !is_first_arg ) string_builder_ << ", ";
    arg_ptr->accept( *this );
    is_first_arg = false;
  }
  string_builder_ << ")";
}

void AstSerializerVisitor::visit( const ArrayLiteralNode& node ) {
  // VIS_GUARD

  string_builder_ << '[';
  bool is_first_pos = true;
  for ( const auto& pos_ptr : node.getPositions() ) {
    if ( !is_first_pos ) string_builder_ << ", ";
    pos_ptr->accept( *this );
    is_first_pos = false;
  }
  string_builder_ << "]";
}

void AstSerializerVisitor::visit( const LiteralExprNode& node ) {
  string_builder_ << std::format( "{}", node.getValue() );
}

void AstSerializerVisitor::visit( const PrimaryIdentifierNode& node ) {
  // VIS_GUARD

  string_builder_ << node.getIdentifier();
}

void AstSerializerVisitor::visit( const ProgramNode& node ) {
  // VisitGuard vis_guard{ string_builder_, PROGRAM_OPENING_SECTION_MARKER, PROGRAM_CLOSING_SECTION_MARKER };

  serializeBlock( node.getStatementList() );
}

std::string AstSerializerVisitor::getString() noexcept {
  std::string result = std::move( string_builder_ ).str();
  flushBuffer();
  return result;
}
