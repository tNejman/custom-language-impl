add_library(Drivers STATIC
    LexerDriver.cc
)

target_link_libraries(Drivers PUBLIC
    Headers
    magic_enum::magic_enum
    argparse::argparse
)