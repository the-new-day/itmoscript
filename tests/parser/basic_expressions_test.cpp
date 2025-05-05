#include "parser_test.hpp"

TEST(ParserTestSuite, SimpleExpressionStringTest) {
    std::string code = R"(
        x1 = x2
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    ASSERT_EQ(program.String(), "x1 = x2");
}

TEST(ParserTestSuite, IdentifierExpressionTest) {
    std::string code = R"(
        foobar
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    std::string ident = expr_stmt->String();
    ASSERT_EQ(ident, "foobar");
}

TEST(ParserTestSuite, IntLiteralExpressionTest) {
    std::string code = R"(
        100500
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);
    ASSERT_NE(expr_stmt->expr, nullptr);

    TestIntegerLiteral(expr_stmt->expr, 100500);
}

TEST(ParserTestSuite, BooleanLiteralExpressionTest) {
    std::string code = R"(
        true
        false
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);

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
