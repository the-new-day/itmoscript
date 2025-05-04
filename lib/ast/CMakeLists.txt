add_library(itmoscript_ast AST.cpp Node.cpp)
target_link_libraries(itmoscript_ast itmoscript_lexer)
