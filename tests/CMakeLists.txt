include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  itmoscript_tests
  language/function_test.cpp
  language/types_test.cpp
  language/loop_test.cpp
  language/branch_test.cpp
  language/illegal_ops_test.cpp

  utils_test.cpp

  lexer/lexer_test.cpp
  lexer/literals_test.cpp
  lexer/syntax_errors_test.cpp

  parser/statements_test.cpp
  parser/expressions_test.cpp
  parser/branch_test.cpp
  parser/functions_test.cpp
  parser/loop_test.cpp
  parser/literals_test.cpp

  evaluation_units/literals_test.cpp
  evaluation_units/prefix_expressions_test.cpp
  evaluation_units/infix_expressions_test.cpp
  evaluation_units/type_system_test.cpp
  evaluation_units/string_operators_test.cpp
  evaluation_units/branch_test.cpp
  evaluation_units/naming_test.cpp
  evaluation_units/functions_test.cpp
  evaluation_units/loop_test.cpp
  evaluation_units/scope_test.cpp
  evaluation_units/assignment_test.cpp
)

target_link_libraries(
  itmoscript_tests
  itmoscript
  GTest::gtest_main
  GTest::gmock_main
)

target_include_directories(itmoscript_tests PUBLIC 
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(itmoscript_tests)