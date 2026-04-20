add_library(Lexer
    Lexer.cc
    LexerDriver.cc
)

target_include_directories(Lexer PUBLIC
    magic_enum::magic_enum
    ${CMAKE_SOURCE_DIR}/include/Lexer
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Lexer PUBLIC
    magic_enum::magic_enum
)