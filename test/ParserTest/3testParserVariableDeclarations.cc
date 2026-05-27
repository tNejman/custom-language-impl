#include <cstddef>

#include "Exceptions/ParserExceptions/InvalidTypeException.hpp"
#include "Exceptions/ParserExceptions/MissingExpressionException.hpp"
#include "Exceptions/ParserExceptions/MissingIdentifierException.hpp"
#include "Exceptions/ParserExceptions/MissingOperatorException.hpp"
#include "Exceptions/ParserExceptions/NotConsumedTokensException.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "Parser/Variable.h"
#include "TestHelperPars.hpp"

void assertVariableDeclaration( const VarOrConstDeclNode* node, const std::string identifier,
                                const Mutability mutability, const Type type ) {
  ASSERT_NE( nullptr, node );
  ASSERT_EQ( identifier, node->getIdentifier() );
  ASSERT_EQ( mutability, node->getMutability() );
  ASSERT_EQ( type, node->getType() );
  ASSERT_NE( nullptr, node->getInitializerExpr() );
}

/* VarDecl ::== "var" Type IDENTIFIER "=" Expression

  var_decl = var type identifier = expr

  ConstDecl ::== Type IDENTIFIER "=" Expression

  const_decl = type identifier = expr
*/
class ParserVariableDeclarationTest : public ParserTest {};

TEST_F( ParserVariableDeclarationTest, variable_declaration ) {
  // var int x = 0
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_VAR,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::OP_ASSIGN,
                                                  { TokenType::INT_LITERAL, 0 },
                                                  TokenType::NEWLINE } );
  auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
  assertVariableDeclaration( var_decl_ptr, "x", Mutability::MUTABLE, Type{ BaseType::INT } );
}

TEST_F( ParserVariableDeclarationTest, constant_declaration ) {
  // int x = 0
  auto program_ptr = initTokensAndBuildProgram( { TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::OP_ASSIGN,
                                                  { TokenType::INT_LITERAL, 0 },
                                                  TokenType::NEWLINE } );
  auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
  assertVariableDeclaration( var_decl_ptr, "x", Mutability::IMMUTABLE, Type{ BaseType::INT } );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_basic_types ) {
  const std::vector<std::tuple<TokenType, BaseType, TokenType, TokenVal>> token_types_base_type = {
      { TokenType::T_INT, BaseType::INT, TokenType::INT_LITERAL, 0 },
      { TokenType::T_FLOAT, BaseType::FLOAT, TokenType::FLOAT_LITERAL, 0.f },
      { TokenType::T_CHAR, BaseType::CHAR, TokenType::CHAR_LITERAL, 'a' },
      { TokenType::T_BOOL, BaseType::BOOL, TokenType::BOOL_LITERAL, true } };
  const std::string identifier = "x";

  for ( const auto [tt, bt, ttl, val] : token_types_base_type ) {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_VAR,
                                                    tt,
                                                    { TokenType::IDENTIFIER, identifier },
                                                    TokenType::OP_ASSIGN,
                                                    { ttl, val },
                                                    TokenType::NEWLINE } );
    auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
    assertVariableDeclaration( var_decl_ptr, identifier, Mutability::MUTABLE, Type{ bt } );
  }
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_basic_type ) {
  const std::vector<std::tuple<TokenType, BaseType, TokenType, TokenVal>> token_types_base_type = {
      { TokenType::T_INT, BaseType::INT, TokenType::INT_LITERAL, 0 },
      { TokenType::T_FLOAT, BaseType::FLOAT, TokenType::FLOAT_LITERAL, 0.f },
      { TokenType::T_CHAR, BaseType::CHAR, TokenType::CHAR_LITERAL, 'a' },
      { TokenType::T_BOOL, BaseType::BOOL, TokenType::BOOL_LITERAL, true } };
  const std::string identifier = "x";

  for ( const auto [tt, bt, ttl, val] : token_types_base_type ) {
    auto program_ptr = initTokensAndBuildProgram(
        { tt, { TokenType::IDENTIFIER, identifier }, TokenType::OP_ASSIGN, { ttl, val }, TokenType::NEWLINE } );
    auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
    assertVariableDeclaration( var_decl_ptr, identifier, Mutability::IMMUTABLE, Type{ bt } );
  }
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_string_type ) {
  // var str x = "abcd"
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_VAR,
                                                  TokenType::T_STR,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::OP_ASSIGN,
                                                  { TokenType::STRING_LITERAL, "abcd" },
                                                  TokenType::NEWLINE } );
  ASSERT_NE( nullptr, program_ptr );
  ASSERT_EQ( 1, program_ptr->getStatementList().size() );
  auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
  assertVariableDeclaration( var_decl_ptr, "x", Mutability::MUTABLE,
                             Type::buildTypeArrayTypeFromBase( BaseType::CHAR ) );
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_string_type ) {
  // str x = "abcd"
  auto program_ptr = initTokensAndBuildProgram( { TokenType::T_STR,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::OP_ASSIGN,
                                                  { TokenType::STRING_LITERAL, "abcd" },
                                                  TokenType::NEWLINE } );
  ASSERT_NE( nullptr, program_ptr );
  ASSERT_EQ( 1, program_ptr->getStatementList().size() );
  auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
  assertVariableDeclaration( var_decl_ptr, "x", Mutability::IMMUTABLE,
                             Type::buildTypeArrayTypeFromBase( BaseType::CHAR ) );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_array_types ) {
  const std::vector<std::tuple<TokenType, BaseType, TokenType, TokenVal>> token_types_base_type = {
      { TokenType::T_INT, BaseType::INT, TokenType::INT_LITERAL, 0 },
      { TokenType::T_FLOAT, BaseType::FLOAT, TokenType::FLOAT_LITERAL, 0.f },
      { TokenType::T_CHAR, BaseType::CHAR, TokenType::CHAR_LITERAL, 'a' },
      { TokenType::T_BOOL, BaseType::BOOL, TokenType::BOOL_LITERAL, true } };
  const std::string identifier = "x";

  for ( const auto [tt, bt, ttl, val] : token_types_base_type ) {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_VAR,
                                                    tt,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    { TokenType::IDENTIFIER, identifier },
                                                    TokenType::OP_ASSIGN,
                                                    { ttl, val },
                                                    TokenType::NEWLINE } );
    auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
    assertVariableDeclaration( var_decl_ptr, identifier, Mutability::MUTABLE, Type::buildTypeArrayTypeFromBase( bt ) );
  }
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_array_type ) {
  const std::vector<std::tuple<TokenType, BaseType, TokenType, TokenVal>> token_types_base_type = {
      { TokenType::T_INT, BaseType::INT, TokenType::INT_LITERAL, 0 },
      { TokenType::T_FLOAT, BaseType::FLOAT, TokenType::FLOAT_LITERAL, 0.f },
      { TokenType::T_CHAR, BaseType::CHAR, TokenType::CHAR_LITERAL, 'a' },
      { TokenType::T_BOOL, BaseType::BOOL, TokenType::BOOL_LITERAL, true } };
  const std::string identifier = "x";

  for ( const auto [tt, bt, ttl, val] : token_types_base_type ) {
    auto program_ptr = initTokensAndBuildProgram( { tt,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    { TokenType::IDENTIFIER, identifier },
                                                    TokenType::OP_ASSIGN,
                                                    { ttl, val },
                                                    TokenType::NEWLINE } );
    auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
    assertVariableDeclaration( var_decl_ptr, identifier, Mutability::IMMUTABLE,
                               Type::buildTypeArrayTypeFromBase( bt ) );
  }
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_with_double_array_types ) {
  const std::vector<std::tuple<TokenType, BaseType, TokenType, TokenVal>> token_types_base_type = {
      { TokenType::T_INT, BaseType::INT, TokenType::INT_LITERAL, 0 },
      { TokenType::T_FLOAT, BaseType::FLOAT, TokenType::FLOAT_LITERAL, 0.f },
      { TokenType::T_CHAR, BaseType::CHAR, TokenType::CHAR_LITERAL, 'a' },
      { TokenType::T_BOOL, BaseType::BOOL, TokenType::BOOL_LITERAL, true } };
  const std::string identifier = "x";

  for ( const auto [tt, bt, ttl, val] : token_types_base_type ) {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_VAR,
                                                    tt,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    { TokenType::IDENTIFIER, identifier },
                                                    TokenType::OP_ASSIGN,
                                                    { ttl, val },
                                                    TokenType::NEWLINE } );
    auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
    assertVariableDeclaration( var_decl_ptr, identifier, Mutability::MUTABLE,
                               Type::buildTypeArrayFromBaseNRec( bt, 2 ) );
  }
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_with_double_array_type ) {
  const std::vector<std::tuple<TokenType, BaseType, TokenType, TokenVal>> token_types_base_type = {
      { TokenType::T_INT, BaseType::INT, TokenType::INT_LITERAL, 0 },
      { TokenType::T_FLOAT, BaseType::FLOAT, TokenType::FLOAT_LITERAL, 0.f },
      { TokenType::T_CHAR, BaseType::CHAR, TokenType::CHAR_LITERAL, 'a' },
      { TokenType::T_BOOL, BaseType::BOOL, TokenType::BOOL_LITERAL, true } };
  const std::string identifier = "x";

  for ( const auto [tt, bt, ttl, val] : token_types_base_type ) {
    auto program_ptr = initTokensAndBuildProgram( { tt,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    { TokenType::IDENTIFIER, identifier },
                                                    TokenType::OP_ASSIGN,
                                                    { ttl, val },
                                                    TokenType::NEWLINE } );
    auto* var_decl_ptr = dynamic_cast<const VarOrConstDeclNode*>( program_ptr->getStatementList()[0].get() );
    assertVariableDeclaration( var_decl_ptr, identifier, Mutability::IMMUTABLE,
                               Type::buildTypeArrayFromBaseNRec( bt, 2 ) );
  }
}

/* variable declaration err
 */

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_type ) {
  // var x = 5
  ASSERT_THROW(
      auto program = initTokensAndBuildProgram(
          { TokenType::KW_VAR, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 5 } } ),
      InvalidTypeException );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_identifier ) {
  // var int = 5
  ASSERT_THROW( auto program = initTokensAndBuildProgram(
                    { TokenType::KW_VAR, TokenType::T_INT, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 5 } } ),
                MissingIdentifierException );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_assign_operator ) {
  // var int x 5
  ASSERT_THROW(
      auto program = initTokensAndBuildProgram(
          { TokenType::KW_VAR, TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, { TokenType::INT_LITERAL, 5 } } ),
      MissingOperatorException );
}

TEST_F( ParserVariableDeclarationTest, variable_declaration_missing_initializer_expression ) {
  // var int x =
  ASSERT_THROW( auto program = initTokensAndBuildProgram(
                    { TokenType::KW_VAR, TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN } ),
                MissingExpressionException );
}

/* constant declaration err
 */

TEST_F( ParserVariableDeclarationTest, constant_declaration_missing_identifier ) {
  // int = 5
  ASSERT_THROW( auto program = initTokensAndBuildProgram(
                    { TokenType::T_INT, TokenType::OP_ASSIGN, { TokenType::INT_LITERAL, 5 } } ),
                MissingIdentifierException );
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_missing_assign_operator ) {
  // int x 5
  ASSERT_THROW( auto program = initTokensAndBuildProgram(
                    { TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, { TokenType::INT_LITERAL, 5 } } ),
                MissingOperatorException );
}

TEST_F( ParserVariableDeclarationTest, constant_declaration_missing_initializer_expression ) {
  // int x =
  ASSERT_THROW( auto program = initTokensAndBuildProgram(
                    { TokenType::T_INT, { TokenType::IDENTIFIER, "x" }, TokenType::OP_ASSIGN } ),
                MissingExpressionException );
}

TEST_F( ParserVariableDeclarationTest, void_type_variable_declaration ) {
  // void x = 5
  ASSERT_THROW( auto program = initTokensAndBuildProgram( { TokenType::T_VOID,
                                                            { TokenType::IDENTIFIER, "x" },
                                                            TokenType::OP_ASSIGN,
                                                            { TokenType::INT_LITERAL, 5 },
                                                            TokenType::NEWLINE } ),
                NotConsumedTokensException );
}
