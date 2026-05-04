add_library(Drivers
    LexerDriver.cc
    Formatter.cc
)

target_include_directories(Drivers PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Drivers PUBLIC
    magic_enum::magic_enum
)