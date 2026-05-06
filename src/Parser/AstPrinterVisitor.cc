#include "Parser/AstPrinterVisitor.h"

#include <iostream>

#include "Drivers/Formatter.hpp"
#include "Parser/ParameterDecl.hpp"

// AstPrinterVisitor::

// void AstPrinterVisitor::printPrefix() const noexcept {
//   for ( size_t i = 0; i < is_last_child_stack_.size(); ++i ) {
//     if ( i == is_last_child_stack_.size() - 1 ) {
//       if ( is_last_child_stack_[i] ) {
//         std::cout << "└── ";
//       } else {
//         std::cout << "├── ";
//       }
//     } else {
//       if ( is_last_child_stack_[i] ) {
//         std::cout << "    ";
//       } else {
//         std::cout << "│   ";
//       }
//     }
//   }
// }

void AstPrinterVisitor::printIndent() const noexcept {
  for ( int i = 0; i < indent_level_; ++i ) {
    std::cout << "  " << char( 200 );
  }
  std::cout << char( 205 ) << char( 205 ) << char( 203 ) << " ";
}

void AstPrinterVisitor::visit( const FunctionDefNode& node ) {
  printIndent();
  std::cout << std::format( "Function Definition at pos {}: type: {} identifier: {}", node.getPosition(),
                            node.getType(), node.getIdentifier() );
  const auto& parameters = node.getParameters();
  if ( parameters.size() == 0 ) {
    std::cout << "parameter list: ()\n";
    return;
  }
  std::cout << "parameter list: (" << std::format( "{}", *( parameters[0] ) );
  for ( int i = 1; i < parameters.size(); ++i ) {
    std::cout << std::format( ", {}", *( parameters[i] ) );
  }
  std::cout << ")\n";
}

void AstPrinterVisitor::visit( const IExpressionNode& node ) {
}

void AstPrinterVisitor::visit( const VarOrConstDeclNode& node ) {
  printIndent();
  const std::string mutability_quantifier = node.getMutability() == Mutability::MUTABLE ? "Variable" : "Constant";
  std::cout << std::format( "{} Declaration at pos: {}, type: {}, identifier: {} = \n", mutability_quantifier,
                            node.getPosition(), node.getType(), node.getIdentifier() );
  ++indent_level_;
  if ( auto init_expr_ptr = node.getInitializerExpr() ) {
    init_expr_ptr->accept( *this );
  }
  --indent_level_;
}

void AstPrinterVisitor::visit( const IfStatementNode& node ) {
}

void AstPrinterVisitor::visit( const WhileStatementNode& node ) {
}

void AstPrinterVisitor::visit( const ControlFlowNode& node ) {
}

void AstPrinterVisitor::visit( const ReturnNode& node ) {
}

void AstPrinterVisitor::visit( const AssignmentExprNode& node ) {
}

void AstPrinterVisitor::visit( const BinaryExprNode& node ) {
}

void AstPrinterVisitor::visit( const UnaryExprNode& node ) {
}

void AstPrinterVisitor::visit( const FunctionCallNode& node ) {
}

void AstPrinterVisitor::visit( const ArrayLiteralNode& node ) {
  printIndent();
  std::cout << std::format( "Array Literal at pos: {}, value: [", node.getPosition() );
  const auto& positions = node.getPositions();
  for ( int i = 0; i < positions.size(); ++i ) {
    if ( i != 0 ) {
      std::cout << ',';
    }
    positions[i]->accept( *this );
  }
  std::cout << "]\n";
}

void AstPrinterVisitor::visit( const LiteralExprNode& node ) {
  printIndent();
  std::cout << std::format( "Literal Expr at pos: {}, type: {}, value: {}\n", node.getPosition(), node.getType(),
                            node.getValue() );
}

void AstPrinterVisitor::visit( const PrimaryIdentifierNode& node ) {
}

void AstPrinterVisitor::visit( const ProgramNode& node ) {
  std::cout << "Program. Statements: \n\n";

  for ( const auto& statement : node.getStatementList() ) {
    statement->accept( *this );
  }
}
