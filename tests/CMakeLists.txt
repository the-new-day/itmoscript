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
  function_test.cpp
  types_test.cpp
  loop_and_branch_test.cpp
  illegal_ops_test.cpp
  lexer/lexer_test.cpp
  lexer/numbers_parsing_test.cpp
  parser/statements_test.cpp
  parser/basic_expressions_test.cpp
  parser/prefix_and_infix_ops_test.cpp
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