add_subdirectory(lexer)
add_subdirectory(repl)

add_library(itmoscript interpreter.cpp)
target_link_libraries(itmoscript itmoscript_lexer itmoscript_repl)
