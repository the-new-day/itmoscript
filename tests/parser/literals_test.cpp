#include "parser_test.hpp"

TEST(ParserLiteralsTestSuite, IntLiteralExpressionTest) {
    std::string code = R"(
        100500
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestIntegerLiteral(expr_stmt->expr, 100500);
}

TEST(ParserLiteralsTestSuite, FloatLiteralExpressionTest) {
    std::string code = R"(
        3.14
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestFloatLiteral(expr_stmt->expr, 3.14);
}

TEST(ParserLiteralsTestSuite, StringLiteralExpressionTest) {
    std::string code = R"(
        "Hello World"
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestStringLiteral(expr_stmt->expr, "Hello World");
}

TEST(ParserLiteralsTestSuite, StringLiteralEscapeSeqTest) {
    std::string code = R"("\n\t\'\"\?\a\b\f\r\v\\")";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestStringLiteral(expr_stmt->expr, "\n\t\'\"\?\a\b\f\r\v\\");
}

TEST(ParserLiteralsTestSuite, NullTypeLiteralExpressionTest) {
    std::string code = R"(
        nil
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestNullTypeLiteral(expr_stmt->expr);
}

TEST(ParserLiteralsTestSuite, BooleanLiteralExpressionTest) {
    std::string code = R"(
        true
        false
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 2);

    auto* expr_stmt1 = dynamic_cast<itmoscript::ast::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt1, nullptr);
    ASSERT_NE(expr_stmt1->expr, nullptr);

    auto* expr_stmt2 = dynamic_cast<itmoscript::ast::ExpressionStatement*>(statements[1].get());
    ASSERT_NE(expr_stmt2, nullptr);
    ASSERT_NE(expr_stmt2->expr, nullptr);

    TestBooleanLiteral(expr_stmt1->expr, true);
    TestBooleanLiteral(expr_stmt2->expr, false);
}
