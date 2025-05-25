#include "parser_test.hpp"

itmoscript::IfExpression* GetIfExpression(const std::unique_ptr<itmoscript::Statement>& stmt) {
    auto* expr_stmt = GetExpressionStatement(stmt);

    auto* if_expr = dynamic_cast<itmoscript::IfExpression*>(expr_stmt->expr.get());
    EXPECT_NE(if_expr, nullptr);
    EXPECT_NE(if_expr->main_condition, nullptr);
    EXPECT_NE(if_expr->main_consequence, nullptr);

    return if_expr;
}

itmoscript::IfExpression* GetIfExpression(const itmoscript::Program& program) {
    const auto& statements = program.GetStatements();
    EXPECT_EQ(statements.size(), 1);

    return GetIfExpression(statements[0]);
}

TEST(ParserTestSuite, SimpleIfExpressionTest) {
    std::string code = R"(
        if foobar > fizzbuzz then 
            aboba 
        end if
    )";

    auto program = GetParsedProgram(code);
    auto* if_expr = GetIfExpression(program);
    TestInfixExpression(if_expr->main_condition, "foobar", ">", "fizzbuzz");

    const auto& block_statements = if_expr->main_consequence->GetStatements();
    ASSERT_EQ(block_statements.size(), 1);

    auto* consequence = GetExpressionStatement(block_statements[0]);
    TestIdentifier(consequence->expr, "aboba");

    ASSERT_EQ(if_expr->alternatives.size(), 0);
}

TEST(ParserTestSuite, SimpleIfElseExpressionTest) {
    std::string code = R"(
        if foobar > fizzbuzz then 
            aboba 
        else 
            bibaboba 
        end if
    )";

    auto program = GetParsedProgram(code);
    auto* if_expr = GetIfExpression(program);

    TestInfixExpression(if_expr->main_condition, "foobar", ">", "fizzbuzz");

    const auto& block_statements = if_expr->main_consequence->GetStatements();
    ASSERT_EQ(block_statements.size(), 1);

    auto* consequence = GetExpressionStatement(block_statements[0]);
    TestIdentifier(consequence->expr, "aboba");

    ASSERT_EQ(if_expr->alternatives.size(), 1);
    ASSERT_EQ(if_expr->alternatives[0].condition, nullptr);

    const auto& alt_block_statements = if_expr->alternatives[0].consequence->GetStatements();
    ASSERT_EQ(alt_block_statements.size(), 1);

    auto* alternative = GetExpressionStatement(alt_block_statements[0]);
    TestIdentifier(alternative->expr, "bibaboba");
}

TEST(ParserTestSuite, SimpleElseIfExpressionTest) {
    std::string code = R"(
        if foobar > fizzbuzz then 
            aboba 
        else if foobar < fizzbuzz then
            bibaboba
        else
            456
        end if
    )";

    auto program = GetParsedProgram(code);
    auto* if_expr = GetIfExpression(program);

    TestInfixExpression(if_expr->main_condition, "foobar", ">", "fizzbuzz");

    const auto& consequence_block_statements = if_expr->main_consequence->GetStatements();
    ASSERT_EQ(consequence_block_statements.size(), 1);

    auto* consequence = GetExpressionStatement(consequence_block_statements[0]);
    TestIdentifier(consequence->expr, "aboba");

    ASSERT_EQ(if_expr->alternatives.size(), 2);
    ASSERT_NE(if_expr->alternatives[0].condition, nullptr);
    ASSERT_EQ(if_expr->alternatives[0].consequence->GetStatements().size(), 1);

    TestInfixExpression(if_expr->alternatives[0].condition, "foobar", "<", "fizzbuzz");

    auto* else_if_consequence = GetExpressionStatement(if_expr->alternatives[0].consequence->GetStatements()[0]);
    TestIdentifier(else_if_consequence->expr, "bibaboba");

    ASSERT_EQ(if_expr->alternatives[1].condition, nullptr);
    ASSERT_EQ(if_expr->alternatives[1].consequence->GetStatements().size(), 1);

    auto* else_consequence = GetExpressionStatement(if_expr->alternatives[1].consequence->GetStatements()[0]);
    TestLiteralExpression(else_consequence->expr, 456);
}
