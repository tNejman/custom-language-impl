#include "Parser/Node.h"

#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"

FunctionDefNode::FunctionDefNode( Position position, std::string identifier, Type ret_type,
                                  std::vector<ParameterDecl> parameters, Block block )
    : INode( position ),
      identifier_( std::move( identifier ) ),
      return_type_( std::move( ret_type ) ),
      parameters_( std::move( parameters ) ),
      block_( std::move( block ) ) {
}

void FunctionDefNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
std::string_view FunctionDefNode::getIdentifier() const noexcept {
  return identifier_;
}
const Type& FunctionDefNode::getType() const noexcept {
  return return_type_;
}
const std::vector<ParameterDecl>& FunctionDefNode::getParameters() const noexcept {
  return parameters_;
}
const Block& FunctionDefNode::getBlock() const noexcept {
  return block_;
}

VarOrConstDeclNode::VarOrConstDeclNode( Position positon, std::string identifier, Mutability mutability, Type type,
                                        std::unique_ptr<IExpressionNode> initializer_expression )
    : INode( positon ),
      identifier_( std::move( identifier ) ),
      mutability_( mutability ),
      variable_decl_type_( std::move( type ) ),
      initializer_expression_( std::move( initializer_expression ) ) {
}
void VarOrConstDeclNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
std::string_view VarOrConstDeclNode::getIdentifier() const noexcept {
  return identifier_;
}
Mutability VarOrConstDeclNode::getMutability() const noexcept {
  return mutability_;
}
const Type& VarOrConstDeclNode::getType() const noexcept {
  return variable_decl_type_;
}
const IExpressionNode* VarOrConstDeclNode::getInitializerExpr() const noexcept {
  return initializer_expression_.get();
}

IfStatementNode::IfStatementNode( Position position, ExprBlockPairVec cond_block_pairs, Block else_block )
    : INode( position ), cond_block_pairs_( std::move( cond_block_pairs ) ), else_block_( std::move( else_block ) ) {
}
void IfStatementNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const ExprBlockPairVec& IfStatementNode::getCondBlockPairs() const noexcept {
  return cond_block_pairs_;
}
const Block& IfStatementNode::getElseBlock() const noexcept {
  return else_block_;
}

WhileStatementNode::WhileStatementNode( Position position, std::unique_ptr<IExpressionNode> condition, Block block )
    : INode( position ), condition_( std::move( condition ) ), block_( std::move( block ) ) {
}
void WhileStatementNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const IExpressionNode* WhileStatementNode::getCondition() const noexcept {
  return condition_.get();
}
const Block& WhileStatementNode::getBlock() const noexcept {
  return block_;
}

ControlFlowNode::ControlFlowNode( Position position, ControlFlowType type ) : INode( position ), type_( type ) {
}
void ControlFlowNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
ControlFlowType ControlFlowNode::getControlFlowType() const noexcept {
  return type_;
}

ReturnNode::ReturnNode( Position position, std::unique_ptr<IExpressionNode> expression )
    : INode( position ), expression_( std::move( expression ) ) {
}
void ReturnNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const IExpressionNode* ReturnNode::getExpression() const noexcept {
  return expression_.get();
}

AssignmentExprNode::AssignmentExprNode( Position position, std::unique_ptr<IExpressionNode> left_operand,
                                        std::unique_ptr<IExpressionNode> right_operand, AssignmentType type )
    : IExpressionNode( position ),
      left_operand_( std::move( left_operand ) ),
      right_operand_( std::move( right_operand ) ),
      assign_op_( type ) {
}
void AssignmentExprNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const IExpressionNode* AssignmentExprNode::getLeftOperand() const noexcept {
  return left_operand_.get();
}
const IExpressionNode* AssignmentExprNode::getRightOperand() const noexcept {
  return right_operand_.get();
}
AssignmentType AssignmentExprNode::getAssignmentType() const noexcept {
  return assign_op_;
}

BinaryExprNode::BinaryExprNode( Position position, std::unique_ptr<IExpressionNode> left_operand,
                                std::unique_ptr<IExpressionNode> right_operand, BinaryOperator op )
    : IExpressionNode( position ),
      left_operand_( std::move( left_operand ) ),
      right_operand_( std::move( right_operand ) ),
      operator_( op ) {
}
void BinaryExprNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const IExpressionNode* BinaryExprNode::getLeftOperand() const noexcept {
  return left_operand_.get();
}
const IExpressionNode* BinaryExprNode::getRightOperand() const noexcept {
  return right_operand_.get();
}
BinaryOperator BinaryExprNode::getOperator() const noexcept {
  return operator_;
}

UnaryExprNode::UnaryExprNode( Position position, std::unique_ptr<IExpressionNode> opernad, UnaryOperator un_operator )
    : IExpressionNode( position ), operand_( std::move( opernad ) ), operator_( un_operator ) {
}
void UnaryExprNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const IExpressionNode* UnaryExprNode::getOperand() const noexcept {
  return operand_.get();
}
UnaryOperator UnaryExprNode::getOperator() const noexcept {
  return operator_;
}

CastExprNode::CastExprNode( Position position, std::unique_ptr<IExpressionNode> expression, Type type )
    : IExpressionNode( position ), expression_( std::move( expression ) ), type_( std::move( type ) ) {
}
void CastExprNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const IExpressionNode* CastExprNode::getExpression() const noexcept {
  return expression_.get();
}
const Type& CastExprNode::getType() const noexcept {
  return type_;
}

FunctionCallNode::FunctionCallNode( Position position, std::string identifier, ExpressionVec arguments )
    : IExpressionNode( position ), identifier_( std::move( identifier ) ), arguments_( std::move( arguments ) ) {
}
void FunctionCallNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
std::string_view FunctionCallNode::getIdentifier() const noexcept {
  return identifier_;
}
const ExpressionVec& FunctionCallNode::getArguments() const noexcept {
  return arguments_;
}

ArrayLiteralNode::ArrayLiteralNode( Position position, ExpressionVec array_positions )
    : IExpressionNode( position ), array_positions_( std::move( array_positions ) ) {
}
void ArrayLiteralNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const ExpressionVec& ArrayLiteralNode::getPositions() const noexcept {
  return array_positions_;
}

LiteralExprNode::LiteralExprNode( Position position, Type type, Value value )
    : IExpressionNode( position ), type_( std::move( type ) ), value_( std::move( value ) ) {
}
void LiteralExprNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const Type& LiteralExprNode::getType() const noexcept {
  return type_;
}
const Value& LiteralExprNode::getValue() const noexcept {
  return value_;
}

PrimaryIdentifierNode::PrimaryIdentifierNode( Position position, std::string identifier )
    : IExpressionNode( position ), identifier_( std::move( identifier ) ) {
}
void PrimaryIdentifierNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
std::string_view PrimaryIdentifierNode::getIdentifier() const noexcept {
  return identifier_;
}

ProgramNode::ProgramNode( Position position, std::vector<std::unique_ptr<INode>> statements,
                          std::vector<std::unique_ptr<FunctionDefNode>> functions )
    : INode( position ), statement_list_( std::move( statements ) ), function_definitions_( std::move( functions ) ) {
}
void ProgramNode::accept( Visitor& v ) const noexcept {
  v.visit( *this );
}
const std::vector<std::unique_ptr<INode>>& ProgramNode::getStatementList() const noexcept {
  return statement_list_;
}
const std::vector<std::unique_ptr<FunctionDefNode>>& ProgramNode::getFunctionList() const noexcept {
  return function_definitions_;
}