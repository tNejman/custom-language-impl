#include "TestHelper.hpp"
#include "TooLongIdentifierException.hpp"

class LexerKeywordsAndIdentifiersTest : public LexerTest {};

TEST_F( LexerKeywordsAndIdentifiersTest, Types ) {
  assertTokensType( "int", { TokenType::T_INT } );
  assertTokensType( "float", { TokenType::T_FLOAT } );
  assertTokensType( "char", { TokenType::T_CHAR } );
  assertTokensType( "bool", { TokenType::T_BOOL } );
  assertTokensType( "string", { TokenType::T_STR } );
  assertTokensType( "void", { TokenType::T_VOID } );
}

TEST_F( LexerKeywordsAndIdentifiersTest, Keywords ) {
  assertTokensType( "if", { TokenType::KW_IF } );
  assertTokensType( "elseif", { TokenType::KW_ELSEIF } );
  assertTokensType( "else", { TokenType::KW_ELSE } );
  assertTokensType( "while", { TokenType::KW_WHILE } );
  assertTokensType( "do", { TokenType::KW_DO } );
  assertTokensType( "done", { TokenType::KW_DONE } );
  assertTokensType( "break", { TokenType::KW_BREAK } );
  assertTokensType( "continue", { TokenType::KW_CONTINUE } );
  assertTokensType( "ret", { TokenType::KW_RET } );

  assertTokensType( "and", { TokenType::OP_AND } );
  assertTokensType( "or", { TokenType::OP_OR } );
  assertTokensType( "not", { TokenType::OP_NOT } );

  assertTokensType( "def", { TokenType::KW_DEF } );
  assertTokensType( "var", { TokenType::KW_VAR } );
  assertTokensType( "copy", { TokenType::KW_COPY } );
  assertTokensType( "cast_to", { TokenType::KW_CAST_TO } );
  assertTokensType( "mut", { TokenType::KW_MUT } );

  assertTokensType( "true", { TokenType::BOOL_LITERAL } );
  assertTokensType( "false", { TokenType::BOOL_LITERAL } );
}

TEST_F( LexerKeywordsAndIdentifiersTest, KeywordAndIdentifierCollison ) {
  assertTokensType( "definition", { TokenType::IDENTIFIER } );
  assertTokensType( "int_val", { TokenType::IDENTIFIER } );
  assertTokensType( "var mut copy", { TokenType::KW_VAR, TokenType::KW_MUT, TokenType::KW_COPY } );
  assertTokensType( "retsalary", { TokenType::IDENTIFIER } );
  assertTokensType( "done_well", { TokenType::IDENTIFIER } );
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