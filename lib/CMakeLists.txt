add_subdirectory(lexer)
add_subdirectory(repl)
add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(evaluation)
add_subdirectory(objects)
add_subdirectory(stdlib)

add_library(itmoscript interpreter.cpp)

target_link_libraries(itmoscript 
    itmoscript_lexer 
    itmoscript_repl 
    itmoscript_parser
    itmoscript_ast
    itmoscript_evaluation
    itmoscript_objects
    itmoscript_stdlib)
