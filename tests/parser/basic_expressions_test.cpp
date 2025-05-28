#include "parser_test.hpp"

TEST(ParserTestSuite, SimpleExpressionStringTest) {
    std::string code = R"(
        x1 = x2
    )";

    auto program = GetParsedProgram(code);
    ASSERT_EQ(program.String(), "x1 = x2");
}

TEST(ParserTestSuite, IdentifierExpressionTest) {
    std::string code = R"(
        foobar
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);

    std::string ident = expr_stmt->String();
    ASSERT_EQ(ident, "foobar");
}
