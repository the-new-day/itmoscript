#include "parser_test.hpp"

TEST(ParserLiteralsTestSuite, IntLiteralTest) {
    std::string code = R"(
        100500
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestIntegerLiteral(expr_stmt->expr, 100500);
}

TEST(ParserLiteralsTestSuite, FloatLiteralTest) {
    std::string code = R"(
        3.14
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestFloatLiteral(expr_stmt->expr, 3.14);
}

TEST(ParserLiteralsTestSuite, StringLiteralTest) {
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

TEST(ParserLiteralsTestSuite, NullTypeLiteralTest) {
    std::string code = R"(
        nil
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    TestNullTypeLiteral(expr_stmt->expr);
}

TEST(ParserLiteralsTestSuite, BooleanLiteralTest) {
    std::string code = R"(
        true
        false
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 2);

    auto* expr_stmt1 = GetExpressionStatement(statements[0]);
    auto* expr_stmt2 = GetExpressionStatement(statements[1]);

    TestBooleanLiteral(expr_stmt1->expr, true);
    TestBooleanLiteral(expr_stmt2->expr, false);
}

TEST(ParserLiteralsTestSuite, ListLiteralTest) {
    std::string code = R"(
        [1, 2, 3]
        []
        [1]
        [1, "hello"]
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 4);

    auto* expr_stmt1 = GetExpressionStatement(statements[0]);
    auto* expr_stmt2 = GetExpressionStatement(statements[1]);
    auto* expr_stmt3 = GetExpressionStatement(statements[2]);
    auto* expr_stmt4 = GetExpressionStatement(statements[3]);

    TestListLiteral(expr_stmt1->expr, {"1", "2", "3"});
    TestListLiteral(expr_stmt2->expr, {});
    TestListLiteral(expr_stmt3->expr, {"1"});
    TestListLiteral(expr_stmt4->expr, {"1", "\"hello\""});
}
