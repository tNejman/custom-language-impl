#include <gtest/gtest.h>

#include "Parser/Types.hpp"

TEST( AuxTest, compare_type_to_base_type ) {
  Type t1{ BaseType::CHAR };
  ASSERT_EQ( t1, BaseType::CHAR );
}

TEST( AuxTest, compare_type_to_another_type_base ) {
  Type t1{ BaseType::CHAR };
  Type t2{ BaseType::CHAR };
  ASSERT_EQ( t1, t2 );
}

TEST( AuxTest, build_type_array_from_base ) {
  Type t1 = Type::buildTypeArrayTypeFromBase( BaseType::CHAR );
  ASSERT_TRUE( std::holds_alternative<ArrayType>( t1.internal_ ) );
  ASSERT_EQ( *( std::get<ArrayType>( t1.internal_ ).element_type_ ), BaseType::CHAR );
}

TEST( AuxTest, compare_array_types ) {
  Type t1 = Type::buildTypeArrayTypeFromBase( BaseType::CHAR );
  ASSERT_EQ( t1, Type{ ArrayType{ std::make_unique<Type>( BaseType::CHAR ) } } );
}

TEST( AuxTest, compare_built_array_types ) {
  Type t1 = Type::buildTypeArrayTypeFromBase( BaseType::CHAR );
  Type t2 = Type::buildTypeArrayTypeFromBase( BaseType::CHAR );
  ASSERT_EQ( t1, t2 );
}