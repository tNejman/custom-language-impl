#include <gtest/gtest.h>

#include "Exceptions/ParserExceptions/InvalidTypeException.hpp"
#include "Exceptions/ParserExceptions/_ParserExceptionInclude.hpp"
#include "Lexer/Token.hpp"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"
#include "Parser/Variable.h"
#include "TestHelperPars.hpp"

void assertFunctionDeclaration( const FunctionDefNode* node, Type expected_type, const std::string expected_id,
                                const int expected_param_count, const int expected_statement_count ) {
  ASSERT_NE( nullptr, node );
  ASSERT_EQ( node->getType(),
             expected_type );  // struct Type supplements operator ==, so must be on the left in case it's base type
  ASSERT_EQ( expected_id, node->getIdentifier() );
  ASSERT_EQ( expected_param_count, node->getParameters().size() );
  ASSERT_EQ( expected_statement_count, node->getBlock().size() );
}

/* FunctionDef ::== "def" ReturnType IDENTIFIER "(" [ ParamList ] ")" Block

  function_def = def ret_t identifier () block
  function_def = def ret_t identifier (param_list) block

*/
class ParserFunctionTest : public ParserTest {};

TEST_F( ParserFunctionTest, no_param_list ) {
  /*
  def void foo() do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 0, 0 );
}

TEST_F( ParserFunctionTest, param_list ) {
  /*
  def void foo(int x) do
  done
  */
  const std::string function_identifier = "foo";
  const std::string param_identifier = "x";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, param_identifier },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 1, 0 );
}

/* Function def err
 */
TEST_F( ParserFunctionTest, missing_type ) {
  /*
  def foo() do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::RPAREN,
      TokenType::KW_DO,   TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), InvalidTypeException );
}

TEST_F( ParserFunctionTest, missing_identifier ) {
  /*
  def int () do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF, TokenType::T_INT,   TokenType::LPAREN,  TokenType::RPAREN,
      TokenType::KW_DO,  TokenType::NEWLINE, TokenType::KW_DONE, TokenType::NEWLINE,
  };
  ASSERT_THROW( initTokensBuildProgramAndSerialize( std::move( init ) ), MissingIdentifierException );
}

TEST_F( ParserFunctionTest, missing_opening_parenthesis ) {
  /*
  def void foo) do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_VOID,  { TokenType::IDENTIFIER, "foo" },
      TokenType::RPAREN,  TokenType::KW_DO,   TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::OPENING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

TEST_F( ParserFunctionTest, missing_closing_parenthesis ) {
  /*
  def int foo( do
  done
  */
  std::vector<TokenInitializer> init = {
      TokenType::KW_DEF,  TokenType::T_VOID,  { TokenType::IDENTIFIER, "foo" },
      TokenType::LPAREN,  TokenType::KW_DO,   TokenType::NEWLINE,
      TokenType::KW_DONE, TokenType::NEWLINE,
  };
  try {
    initTokensBuildProgramAndSerialize( std::move( init ) );
    FAIL() << "Expected 'MissingParenthesisException' was not thrown";
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::CLOSING, e.getParenthesisType() );
  } catch ( const std::runtime_error& re ) {
    FAIL() << "Expected 'MissingParenthesisException' but a different exception was thrown, with msg: " << re.what();
  }
}

/* ReturnType ::== Type | "void"

 r_type = type
 r_type = void
 */
class ParserReturnTypeTest : public ParserTest {};

TEST_F( ParserReturnTypeTest, void_type ) {
  /*
  def void foo() do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 0, 0 );
}

TEST_F( ParserReturnTypeTest, base_type ) {
  /*
  def T foo() do
  done
  */
  std::array<std::pair<TokenType, BaseType>, 4> tt_type_bt = { { { TokenType::T_INT, BaseType::INT },
                                                                 { TokenType::T_BOOL, BaseType::BOOL },
                                                                 { TokenType::T_CHAR, BaseType::CHAR },
                                                                 { TokenType::T_FLOAT, BaseType::FLOAT } } };

  const std::string function_identifier = "foo";

  for ( const auto [tt, bt] : tt_type_bt ) {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                    tt,
                                                    { TokenType::IDENTIFIER, function_identifier },
                                                    TokenType::LPAREN,
                                                    TokenType::RPAREN,
                                                    TokenType::KW_DO,
                                                    TokenType::NEWLINE,
                                                    TokenType::KW_DONE,
                                                    TokenType::NEWLINE } );
    auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
    assertFunctionDeclaration( fun_def_ptr, Type{ bt }, function_identifier, 0, 0 );
  }
}

TEST_F( ParserReturnTypeTest, array_from_base_type ) {
  /*
  def T[] foo() do
  done
  */
  std::array<std::pair<TokenType, BaseType>, 4> tt_type_bt = { { { TokenType::T_INT, BaseType::INT },
                                                                 { TokenType::T_BOOL, BaseType::BOOL },
                                                                 { TokenType::T_CHAR, BaseType::CHAR },
                                                                 { TokenType::T_FLOAT, BaseType::FLOAT } } };

  const std::string function_identifier = "foo";

  for ( const auto [tt, bt] : tt_type_bt ) {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                    tt,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    { TokenType::IDENTIFIER, function_identifier },
                                                    TokenType::LPAREN,
                                                    TokenType::RPAREN,
                                                    TokenType::KW_DO,
                                                    TokenType::NEWLINE,
                                                    TokenType::KW_DONE,
                                                    TokenType::NEWLINE } );
    auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
    assertFunctionDeclaration( fun_def_ptr, Type::buildTypeArrayTypeFromBase( bt ), function_identifier, 0, 0 );
  }
}

TEST_F( ParserReturnTypeTest, double_array_from_base_type ) {
  /*
  def T[][] foo() do
  done
  */
  std::array<std::pair<TokenType, BaseType>, 4> tt_type_bt = { { { TokenType::T_INT, BaseType::INT },
                                                                 { TokenType::T_BOOL, BaseType::BOOL },
                                                                 { TokenType::T_CHAR, BaseType::CHAR },
                                                                 { TokenType::T_FLOAT, BaseType::FLOAT } } };

  const std::string function_identifier = "foo";

  for ( const auto [tt, bt] : tt_type_bt ) {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                    tt,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    TokenType::LBRACKET,
                                                    TokenType::RBRACKET,
                                                    { TokenType::IDENTIFIER, function_identifier },
                                                    TokenType::LPAREN,
                                                    TokenType::RPAREN,
                                                    TokenType::KW_DO,
                                                    TokenType::NEWLINE,
                                                    TokenType::KW_DONE,
                                                    TokenType::NEWLINE } );
    auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
    assertFunctionDeclaration( fun_def_ptr, Type::buildTypeArrayFromBaseNRec( bt, 2 ), function_identifier, 0, 0 );
  }
}

/* Return type errors

*/

TEST_F( ParserReturnTypeTest, void_array_type ) {
  /*
  def void[] foo() do
  done
  */
  const std::string function_identifier = "foo";
  ASSERT_THROW( auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                                TokenType::T_VOID,
                                                                TokenType::LBRACKET,
                                                                TokenType::RBRACKET,
                                                                { TokenType::IDENTIFIER, function_identifier },
                                                                TokenType::LPAREN,
                                                                TokenType::RPAREN,
                                                                TokenType::KW_DO,
                                                                TokenType::NEWLINE,
                                                                TokenType::KW_DONE } ),
                MissingIdentifierException );
}

/* ParamList ::== Param { "," Param }

  param_list = param 0*(,param)
  param_list = param 1*(,param)
  param_list = param 2+*(,param)
*/
class ParserParamListTest : public ParserTest {};

TEST_F( ParserParamListTest, param_then_zero_params ) {
  /*
  def void foo(int x) do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 1, 0 );
}

TEST_F( ParserParamListTest, param_then_one_param ) {
  /*
  def void foo(int x, int y) do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::COMMA,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "y" },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  ASSERT_NE( nullptr, fun_def_ptr );
  ASSERT_EQ( 2, fun_def_ptr->getParameters().size() );
}

TEST_F( ParserParamListTest, param_then_more_than_one_param ) {
  /*
  def void foo(int x, int y, int z) do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::COMMA,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "y" },
                                                  TokenType::COMMA,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "z" },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  ASSERT_NE( nullptr, fun_def_ptr );
  ASSERT_EQ( 3, fun_def_ptr->getParameters().size() );
}

/* Param List errors

*/

TEST_F( ParserParamListTest, params_not_separated_by_comma ) {
  /*
  def void foo(int x int y) do
  done
  */
  const std::string function_identifier = "foo";
  try {
    auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                    TokenType::T_VOID,
                                                    { TokenType::IDENTIFIER, function_identifier },
                                                    TokenType::LPAREN,
                                                    TokenType::T_INT,
                                                    { TokenType::IDENTIFIER, "x" },
                                                    TokenType::T_INT,
                                                    { TokenType::IDENTIFIER, "y" },
                                                    TokenType::RPAREN,
                                                    TokenType::KW_DO,
                                                    TokenType::NEWLINE,
                                                    TokenType::KW_DONE,
                                                    TokenType::NEWLINE } );
  } catch ( const MissingParenthesisException& e ) {
    ASSERT_EQ( ParenthesisType::CLOSING, e.getParenthesisType() );
  } catch ( ... ) {
    FAIL() << "Something different thrown";
  }
}

/* Param ::== [ ParamModifier ] Type IDENTIFIER

  param = type identifier
  param = param_mod type identifier
*/
class ParserParamTest : public ParserTest {};

TEST_F( ParserParamTest, type_identifier ) {
  /*
  def void foo(int x) do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 1, 0 );
  auto compr = ParameterDecl{ "x", Type{ BaseType::INT }, PassMode::REFERENCE, Mutability::IMMUTABLE };
  ASSERT_EQ( compr, *( fun_def_ptr->getParameters()[0] ) );
}

TEST_F( ParserParamTest, modifier_copy_type_identifier ) {
  /*
  def void foo(copy int x) do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::KW_COPY,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 1, 0 );
  auto compr = ParameterDecl{ "x", Type{ BaseType::INT }, PassMode::COPY, Mutability::IMMUTABLE };
  ASSERT_EQ( compr, *( fun_def_ptr->getParameters()[0] ) );
}

TEST_F( ParserParamTest, modifier_mut_type_identifier ) {
  /*
  def void foo(mut int x) do
  done
  */
  const std::string function_identifier = "foo";
  auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                  TokenType::T_VOID,
                                                  { TokenType::IDENTIFIER, function_identifier },
                                                  TokenType::LPAREN,
                                                  TokenType::KW_MUT,
                                                  TokenType::T_INT,
                                                  { TokenType::IDENTIFIER, "x" },
                                                  TokenType::RPAREN,
                                                  TokenType::KW_DO,
                                                  TokenType::NEWLINE,
                                                  TokenType::KW_DONE,
                                                  TokenType::NEWLINE } );
  auto* fun_def_ptr = dynamic_cast<const FunctionDefNode*>( program_ptr->getStatementList()[0].get() );
  assertFunctionDeclaration( fun_def_ptr, Type{ BaseType::VOID }, function_identifier, 1, 0 );
  auto compr = ParameterDecl{ "x", Type{ BaseType::INT }, PassMode::REFERENCE, Mutability::MUTABLE };
  ASSERT_EQ( compr, *( fun_def_ptr->getParameters()[0] ) );
}

/* Errors

*/

TEST_F( ParserParamTest, two_modifiers ) {
  /*
  def void foo(mut copy int x) do
  done
  */
  const std::string function_identifier = "foo";
  ASSERT_THROW( auto program_ptr = initTokensAndBuildProgram( { TokenType::KW_DEF,
                                                                TokenType::T_VOID,
                                                                { TokenType::IDENTIFIER, function_identifier },
                                                                TokenType::LPAREN,
                                                                TokenType::KW_MUT,
                                                                TokenType::KW_COPY,
                                                                TokenType::T_INT,
                                                                { TokenType::IDENTIFIER, "x" },
                                                                TokenType::RPAREN,
                                                                TokenType::KW_DO,
                                                                TokenType::NEWLINE,
                                                                TokenType::KW_DONE,
                                                                TokenType::NEWLINE } ),
                InvalidTypeException );
}