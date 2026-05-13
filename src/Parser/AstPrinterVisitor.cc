#include "Parser/AstPrinterVisitor.h"

#include <cmath>
#include <print>
#include <stdexcept>

#include "Drivers/Formatter.hpp"
#include "Parser/Node.h"

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
    std::print( "  {}", char( 200 ) );
  }
  std::print( "{}{}{} ", char( 205 ), char( 205 ), char( 203 ) );
}

void AstPrinterVisitor::visit( const FunctionDefNode& node ) {
  printIndent();
  std::print( "Function Definition at pos {}: type: {} identifier: {}", node.getPosition(), node.getType(),
              node.getIdentifier() );
  std::print( ", parameter list: (" );
  bool first = true;
  for ( const auto& param_ptr : node.getParameters() ) {
    if ( !first ) std::print( ", " );
    std::print( "{}", *param_ptr );
    first = false;
  }
  std::println( ")" );

  printIndent();
  std::println( "Block:" );
  ++indent_level_;
  for ( const auto& stmnt_ptr : node.getBlock() ) {
    stmnt_ptr->accept( *this );
  }
  --indent_level_;
}

void AstPrinterVisitor::visit( const VarOrConstDeclNode& node ) {
  printIndent();
  const std::string_view mutability_quantifier = node.getMutability() == Mutability::MUTABLE ? "Variable" : "Constant";
  std::println( "{} Declaration at pos: {}, type: {}, identifier: {} = ", mutability_quantifier, node.getPosition(),
                node.getType(), node.getIdentifier() );
  ++indent_level_;
  if ( auto init_expr_ptr = node.getInitializerExpr() ) {
    init_expr_ptr->accept( *this );
  }
  --indent_level_;
}

void AstPrinterVisitor::visit( const IfStatementNode& node ) {
  printIndent();
  std::println( "If Statement at pos: {}", node.getPosition() );
  ++indent_level_;

  bool is_first_branch = true;
  for ( const auto& [cond_ptr, block] : node.getCondBlockPairs() ) {
    printIndent();
    if ( is_first_branch ) {
      std::println( "If Branch:" );
      is_first_branch = false;
    } else {
      std::println( "Else If Branch:" );
    }
    ++indent_level_;

    printIndent();
    std::println( " Condition:" );
    ++indent_level_;
    cond_ptr->accept( *this );
    --indent_level_;

    printIndent();
    std::println( " Block:" );
    ++indent_level_;
    for ( const auto& block_stmt_ptr : block ) {
      block_stmt_ptr->accept( *this );
    }
    --indent_level_;
    --indent_level_;
  }

  printIndent();
  std::println( "Else Block:" );
  for ( const auto& stmnt_ptr : node.getElseBlock() ) {
    stmnt_ptr->accept( *this );
  }

  --indent_level_;
}

void AstPrinterVisitor::visit( const WhileStatementNode& node ) {
  printIndent();
  std::println( "While Statement at pos: {}", node.getPosition() );
  ++indent_level_;

  printIndent();
  std::println( "Condition:" );
  ++indent_level_;
  node.getCondition()->accept( *this );
  --indent_level_;
  printIndent();
  std::println( "Block:" );
  ++indent_level_;
  for ( const auto& statement_ptr : node.getBlock() ) {
    statement_ptr->accept( *this );
  }
  --indent_level_;
  --indent_level_;
}

void AstPrinterVisitor::visit( const ControlFlowNode& node ) {
  printIndent();
  std::println( "Control Flow at pos: {} instruction: {}", node.getPosition(),
                magic_enum::enum_name( node.getControlFlowType() ) );
}

void AstPrinterVisitor::visit( const ReturnNode& node ) {
  printIndent();
  std::println( "Return Statement at pos: {}", node.getPosition() );
  ++indent_level_;

  printIndent();
  std::print( "Initializing expression: " );
  node.getExpression()->accept( *this );
  --indent_level_;
}

void AstPrinterVisitor::visit( const AssignmentExprNode& node ) {
  printIndent();
  std::println( "Assignment Expression at pos: {}", node.getPosition() );
  ++indent_level_;

  printIndent();
  std::print( "Assigned to: " );
  node.getLeftOperand()->accept( *this );

  printIndent();
  std::print( "Operator: {}", magic_enum::enum_name( node.getAssignmentType() ) );

  printIndent();
  std::print( "Assigned value: " );
  node.getRightOperand()->accept( *this );
  --indent_level_;
}

void AstPrinterVisitor::visit( const BinaryExprNode& node ) {
  printIndent();
  std::println( "Binary Expression at pos: {}", node.getPosition() );
  ++indent_level_;

  printIndent();
  std::print( "Left Operand: " );
  node.getLeftOperand()->accept( *this );

  printIndent();
  std::print( "Operator: {}", magic_enum::enum_name( node.getOperator() ) );

  printIndent();
  std::print( "Right Operand: " );
  node.getRightOperand()->accept( *this );
  --indent_level_;
}

void AstPrinterVisitor::visit( const UnaryExprNode& node ) {
  printIndent();
  std::println( "Binary Expression at pos: {}", node.getPosition() );
  ++indent_level_;

  printIndent();
  std::print( "Operator: {}", magic_enum::enum_name( node.getOperator() ) );

  printIndent();
  std::print( "Operand: " );
  node.getOperand()->accept( *this );
  --indent_level_;
}

void AstPrinterVisitor::visit( const CastExprNode& node ) {
  throw std::runtime_error( "AstPrinterVisitor::visit(CastExprNode&) is not implemented" );
}

void AstPrinterVisitor::visit( const FunctionCallNode& node ) {
  printIndent();
  std::print( "Function Call at pos: {}, identifier: {}", node.getPosition(), node.getIdentifier() );
  std::println( ", call argument list: (" );
  bool first = true;
  ++indent_level_;
  for ( const auto& call_arg_ptr : node.getArguments() ) {
    if ( !first ) std::print( ", " );
    call_arg_ptr->accept( *this );
    first = false;
  }
  --indent_level_;
  std::println( ")" );
}

void AstPrinterVisitor::visit( const ArrayLiteralNode& node ) {
  printIndent();
  std::print( "Array Literal at pos: {}, value: [", node.getPosition() );
  bool first_pos = true;
  for ( const auto& pos_ptr : node.getPositions() ) {
    if ( !first_pos ) {
      std::print( ", " );
    }
    if ( pos_ptr ) {
      pos_ptr->accept( *this );
    }
    first_pos = false;
  }
  std::println( "]" );
}

void AstPrinterVisitor::visit( const LiteralExprNode& node ) {
  printIndent();
  std::println( "Literal Expr at pos: {}, type: {}, value: {}", node.getPosition(), node.getType(), node.getValue() );
}

void AstPrinterVisitor::visit( const PrimaryIdentifierNode& node ) {
  printIndent();
  std::println( "Primary Identifier at pos: {}, name: {}", node.getPosition(), node.getIdentifier() );
}

void AstPrinterVisitor::visit( const ProgramNode& node ) {
  std::println( "Program. Statements: " );

  for ( const auto& statement : node.getStatementList() ) {
    statement->accept( *this );
  }
}
