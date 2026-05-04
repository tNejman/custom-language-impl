add_library(Lexer
    Lexer.cc
)

target_include_directories(Lexer PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Lexer PUBLIC 
    magic_enum::magic_enum
)