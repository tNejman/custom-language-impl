add_library(Parser
    AstPrinterVisitor.cc
    AstSerializerVisitor.cc
    Node.cc
    Parser.cc
    ParserHelper.cc
    Variable.cc
)

target_link_libraries(Parser PUBLIC 
    Headers
    magic_enum::magic_enum
)