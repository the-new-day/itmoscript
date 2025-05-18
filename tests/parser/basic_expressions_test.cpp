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

TEST(ParserTestSuite, IntLiteralExpressionTest) {
    std::string code = R"(
        100500
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestIntegerLiteral(expr_stmt->expr, 100500);
}

TEST(ParserTestSuite, FloatLiteralExpressionTest) {
    std::string code = R"(
        3.14
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestFloatLiteral(expr_stmt->expr, 3.14);
}

TEST(ParserTestSuite, StringLiteralExpressionTest) {
    std::string code = R"(
        "Hello World"
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestStringLiteral(expr_stmt->expr, "Hello World");
}

TEST(ParserTestSuite, StringLiteralEscapeSeqTest) {
    std::string code = R"("\n\t\'\"\?\a\b\f\r\v\\")";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestStringLiteral(expr_stmt->expr, "\n\t\'\"\?\a\b\f\r\v\\");
}

TEST(ParserTestSuite, NullTypeLiteralExpressionTest) {
    std::string code = R"(
        nil
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestNullTypeLiteral(expr_stmt->expr);
}

TEST(ParserTestSuite, BooleanLiteralExpressionTest) {
    std::string code = R"(
        true
        false
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 2);

    auto* expr_stmt1 = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt1, nullptr);
    ASSERT_NE(expr_stmt1->expr, nullptr);

    auto* expr_stmt2 = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[1].get());
    ASSERT_NE(expr_stmt2, nullptr);
    ASSERT_NE(expr_stmt2->expr, nullptr);

    TestBooleanLiteral(expr_stmt1->expr, true);
    TestBooleanLiteral(expr_stmt2->expr, false);
}
