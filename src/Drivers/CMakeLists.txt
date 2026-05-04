add_library(Drivers
    LexerDriver.cc
    Formatter.cc
)

target_link_libraries(Drivers PUBLIC
    Headers
    magic_enum::magic_enum
    argparse::argparse
)