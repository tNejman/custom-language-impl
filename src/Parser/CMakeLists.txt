add_library(Parser STATIC
    AstPrinterVisitor.cc
    AstSerializerVisitor.cc
    Node.cc
    Parser.cc
    ParserHelper.cc
)

target_link_libraries(Parser PUBLIC 
    Headers
    magic_enum::magic_enum
)