#include "parser_test.hpp"

TEST(ParserTestSuite, IfExpressionTest) {
    std::string code = "if foobar > fizzbuzz then aboba end if";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);
    ASSERT_NE(expr_stmt->expr, nullptr);

    auto* if_expr = dynamic_cast<ItmoScript::IfExpression*>(expr_stmt->expr.get());
    ASSERT_NE(if_expr, nullptr);
    ASSERT_NE(if_expr->condition, nullptr);
    ASSERT_NE(if_expr->consequence, nullptr);

    TestInfixExpression(if_expr->condition, "foobar", ">", "fizzbuzz");

    ASSERT_EQ(if_expr->consequence->GetStatements().size(), 1);

    auto* consequence_block = dynamic_cast<ItmoScript::BlockStatement*>(if_expr->consequence.get());
    ASSERT_NE(consequence_block, nullptr);

    const auto& block_statements = consequence_block->GetStatements();
    ASSERT_EQ(block_statements.size(), 1);

    auto* consequence = dynamic_cast<ItmoScript::ExpressionStatement*>(block_statements[0].get());
    TestIdentifier(consequence->expr, "aboba");

    ASSERT_EQ(if_expr->alternative, nullptr);
}

TEST(ParserTestSuite, IfElseExpressionTest) {
    std::string code = R"(
        if foobar > fizzbuzz then aboba else bibaboba end if
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

    auto* if_expr = dynamic_cast<ItmoScript::IfExpression*>(expr_stmt->expr.get());
    ASSERT_NE(if_expr, nullptr);
    ASSERT_NE(if_expr->condition, nullptr);
    ASSERT_NE(if_expr->consequence, nullptr);
    ASSERT_NE(if_expr->alternative, nullptr);

    TestInfixExpression(if_expr->condition, "foobar", ">", "fizzbuzz");

    ASSERT_EQ(if_expr->consequence->GetStatements().size(), 1);

    auto* consequence_block = dynamic_cast<ItmoScript::BlockStatement*>(if_expr->consequence.get());
    ASSERT_NE(consequence_block, nullptr);

    const auto& block_statements = consequence_block->GetStatements();
    ASSERT_EQ(block_statements.size(), 1);

    auto* consequence = dynamic_cast<ItmoScript::ExpressionStatement*>(block_statements[0].get());
    TestIdentifier(consequence->expr, "aboba");

    auto* alternative_block = dynamic_cast<ItmoScript::BlockStatement*>(if_expr->alternative.get());
    ASSERT_NE(alternative_block, nullptr);

    const auto& alt_block_statements = alternative_block->GetStatements();
    ASSERT_EQ(alt_block_statements.size(), 1);

    auto* alternative = dynamic_cast<ItmoScript::ExpressionStatement*>(alt_block_statements[0].get());
    TestIdentifier(alternative->expr, "bibaboba");
}
