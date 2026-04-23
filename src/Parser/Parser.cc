#include "Parser/Parser.h"

void Parser::nextToken() {
    this->current_token_ = lexer_.getNextToken();
}

std::unique_ptr<INode> Parser::buildStatement() {
    nextToken();
    std::unique_ptr<INode> node_;
    switch (current_token_.type_) {
        case TokenType::END_OF_FILE: break;
        case TokenType::KW_DEF: node_ = buildFunctionDef(); break;
        case TokenType::KW_VAR: node_ = buildVariableDecl(true); break;
        case TokenType::T_INT:
        case TokenType::T_FLOAT:
        case TokenType::T_CHAR:
        case TokenType::T_STR:
        case TokenType::T_BOOL: node_ = buildVariableDecl(false); break;
        case TokenType::KW_IF: node_ = buildIfStmt(); break;
        case TokenType::KW_WHILE: node_ = buildWhileStmt(); break;
        case TokenType::KW_BREAK:
        case TokenType::KW_CONTINUE:
        case TokenType::KW_RET: node_ = buildControlFlowStmt(); break;
        case TokenType::LBRACKET: node_ = handleBracketAmbiguity(); break;
        default: node_ = buildExpressionStmt(); break;
    }

    nextToken();
    if (current_token_.type_ != TokenType::NEWLINE && current_token_.type_ != TokenType::END_OF_FILE) {
        throw UnterminatedStatemetException("");
    }
    return node_;
}

std::unique_ptr<INode> Parser::buildFunctionDef() {
    std::unique_ptr<INode> function_node = std::make_unique<FunctionDefNode>();
    nextToken();
    // (Type | "void")
    nextToken();
    // identifier
    nextToken();
    // lparen
    nextToken();
    // ParamList
    nextToken();
    // rparen
    nextToken();
    // kw do
    nextToken();
    // statement list
    nextToken();
    // done
    return function_node;
}

std::unique_ptr<INode> Parser::buildVariableDecl(const bool is_mutable) {
    std::unique_ptr<INode> variable_decl_node = std::make_unique<VariableDeclNode>();
    if (!is_mutable) nextToken();
    // type
    nextToken();
    // identifier
    nextToken();
    // op assign
    nextToken();
    // expression
    return variable_decl_node;
}

std::unique_ptr<INode> Parser::buildIfStmt() {
    std::unique_ptr<INode> variable_decl_node
}

std::unique_ptr<INode> Parser::parse() {
    return buildStatement();
};