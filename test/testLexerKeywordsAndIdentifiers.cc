#include "Exceptions/LexerExceptions/TooLongIdentifierException.hpp"
#include "TestHelper.hpp"


class LexerKeywordsAndIdentifiersTest : public LexerTest {};

TEST_F( LexerKeywordsAndIdentifiersTest, Types ) {
  assertTokensType( "int", { TokenType::T_INT, TokenType::END_OF_FILE } );
  assertTokensType( "float", { TokenType::T_FLOAT, TokenType::END_OF_FILE } );
  assertTokensType( "char", { TokenType::T_CHAR, TokenType::END_OF_FILE } );
  assertTokensType( "bool", { TokenType::T_BOOL, TokenType::END_OF_FILE } );
  assertTokensType( "string", { TokenType::T_STR, TokenType::END_OF_FILE } );
  assertTokensType( "void", { TokenType::T_VOID, TokenType::END_OF_FILE } );
}

TEST_F( LexerKeywordsAndIdentifiersTest, Keywords ) {
  assertTokensType( "if", { TokenType::KW_IF, TokenType::END_OF_FILE } );
  assertTokensType( "elseif", { TokenType::KW_ELSEIF, TokenType::END_OF_FILE } );
  assertTokensType( "else", { TokenType::KW_ELSE, TokenType::END_OF_FILE } );
  assertTokensType( "while", { TokenType::KW_WHILE, TokenType::END_OF_FILE } );
  assertTokensType( "do", { TokenType::KW_DO, TokenType::END_OF_FILE } );
  assertTokensType( "done", { TokenType::KW_DONE, TokenType::END_OF_FILE } );
  assertTokensType( "break", { TokenType::KW_BREAK, TokenType::END_OF_FILE } );
  assertTokensType( "continue", { TokenType::KW_CONTINUE, TokenType::END_OF_FILE } );
  assertTokensType( "ret", { TokenType::KW_RET, TokenType::END_OF_FILE } );

  assertTokensType( "and", { TokenType::OP_AND, TokenType::END_OF_FILE } );
  assertTokensType( "or", { TokenType::OP_OR, TokenType::END_OF_FILE } );
  assertTokensType( "not", { TokenType::OP_NOT, TokenType::END_OF_FILE } );

  assertTokensType( "def", { TokenType::KW_DEF, TokenType::END_OF_FILE } );
  assertTokensType( "var", { TokenType::KW_VAR, TokenType::END_OF_FILE } );
  assertTokensType( "copy", { TokenType::KW_COPY, TokenType::END_OF_FILE } );
  assertTokensType( "cast_to", { TokenType::KW_CAST_TO, TokenType::END_OF_FILE } );
  assertTokensType( "mut", { TokenType::KW_MUT, TokenType::END_OF_FILE } );

  assertTokensType( "true", { TokenType::BOOL_LITERAL, TokenType::END_OF_FILE } );
  assertTokensType( "false", { TokenType::BOOL_LITERAL, TokenType::END_OF_FILE } );
}

TEST_F( LexerKeywordsAndIdentifiersTest, KeywordAndIdentifierCollison ) {
  assertTokensType( "definition", { TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
  assertTokensType( "int_val", { TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
  assertTokensType( "var mut copy",
                    { TokenType::KW_VAR, TokenType::KW_MUT, TokenType::KW_COPY, TokenType::END_OF_FILE } );
  assertTokensType( "retsalary", { TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
  assertTokensType( "done_well", { TokenType::IDENTIFIER, TokenType::END_OF_FILE } );
}

TEST_F( LexerKeywordsAndIdentifiersTest, TooLongIdentifier ) {
  std::stringstream ss{
      "calculateTotalAnnualProjectedRevenueAdjustedForInflationaryPressureMarketVolatilityAndSeasonalFluctuationsWithin"
      "TheNorthernHemisphereRetailDivision()" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken();, TooLongIdentifierException );
}

TEST_F( LexerKeywordsAndIdentifiersTest, TooLongIdentifierAllExtraCharacters ) {
  std::stringstream ss{
      "internal_state_buffer_0123456789_alpha_bravo_charlie_delta_echo_foxtrot_golf_hotel_india_juliet_kilo_lima_mike_"
      "november_oscar_papa_quebec_romeo_sierra_toro_uniform_victor_whiskey_xray_yankee_zulu_STRETCH_GOAL_MAX_CAPACITY_"
      "LIMIT_REACHED_TERMINATE_LEX_0000" };
  Lexer lexer{ ss };
  ASSERT_THROW( lexer.getNextToken();, TooLongIdentifierException );
}