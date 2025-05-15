#include "parser_test.hpp"

ItmoScript::IfExpression* GetIfExpression(const ItmoScript::Program& program, bool has_alternative = false) {
    const auto& statements = program.GetStatements();
    EXPECT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);

    auto* if_expr = dynamic_cast<ItmoScript::IfExpression*>(expr_stmt->expr.get());
    EXPECT_NE(if_expr, nullptr);
    EXPECT_NE(if_expr->condition, nullptr);
    EXPECT_NE(if_expr->consequence, nullptr);
    
    if (has_alternative) {
        EXPECT_NE(if_expr->alternative, nullptr);
    }

    return if_expr;
}

TEST(ParserTestSuite, SimpleIfExpressionTest) {
    std::string code = R"(
        if foobar > fizzbuzz then 
            aboba 
        end if
    )";

    auto program = GetParsedProgram(code);
    auto* if_expr = GetIfExpression(program);
    TestInfixExpression(if_expr->condition, "foobar", ">", "fizzbuzz");

    const auto& block_statements = if_expr->consequence->GetStatements();
    ASSERT_EQ(block_statements.size(), 1);

    auto* consequence = GetExpressionStatement(block_statements[0]);
    TestIdentifier(consequence->expr, "aboba");

    ASSERT_EQ(if_expr->alternative, nullptr);
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

    TestInfixExpression(if_expr->condition, "foobar", ">", "fizzbuzz");

    const auto& block_statements = if_expr->consequence->GetStatements();
    ASSERT_EQ(block_statements.size(), 1);

    auto* consequence = GetExpressionStatement(block_statements[0]);
    TestIdentifier(consequence->expr, "aboba");

    const auto& alt_block_statements = if_expr->alternative->GetStatements();
    ASSERT_EQ(alt_block_statements.size(), 1);

    auto* alternative = GetExpressionStatement(alt_block_statements[0]);
    TestIdentifier(alternative->expr, "bibaboba");
}
