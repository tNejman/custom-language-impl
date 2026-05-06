add_library(Parser
    Parser.cc
    Variable.cc
    ParserHelper.cc
    Node.cc
    AstPrinterVisitor.cc
)

target_link_libraries(Parser PUBLIC 
    Headers
    magic_enum::magic_enum
)