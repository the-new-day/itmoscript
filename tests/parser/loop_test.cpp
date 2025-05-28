#include "parser_test.hpp"

// Code must contain only while statement
itmoscript::ast::WhileStatement* GetWhileStmt(const itmoscript::ast::Program& program) {
    const auto& statements = program.GetStatements();
    EXPECT_EQ(statements.size(), 1);

    auto* while_stmt = dynamic_cast<itmoscript::ast::WhileStatement*>(statements[0].get());
    EXPECT_NE(while_stmt, nullptr);
    EXPECT_NE(while_stmt->condition, nullptr);
    EXPECT_NE(while_stmt->body, nullptr);

    return while_stmt;
}

// Code must contain only for statement
itmoscript::ast::ForStatement* GetForStmt(const itmoscript::ast::Program& program) {
    const auto& statements = program.GetStatements();
    EXPECT_EQ(statements.size(), 1);

    auto* for_stmt = dynamic_cast<itmoscript::ast::ForStatement*>(statements[0].get());
    EXPECT_NE(for_stmt, nullptr);
    EXPECT_NE(for_stmt->range, nullptr);
    EXPECT_NE(for_stmt->body, nullptr);

    return for_stmt;
}

TEST(ParserLoopTestSuite, WhileSimpleTest) {
    std::string code = R"(
        while i < 10
            i = i + 1
            continue
            break
        end while
    )";

    auto program = GetParsedProgram(code);
    auto* while_stmt = GetWhileStmt(program);
    TestInfixExpression(while_stmt->condition, "i", itmoscript::TokenType::kLess, 10);

    std::vector<std::string> expected = {
        "i = (i + 1)", "continue", "break"
    };

    const auto& block_statements = while_stmt->body->GetStatements();
    ASSERT_EQ(block_statements.size(), expected.size());

    TestStatement<itmoscript::ast::AssignStatement>(block_statements[0], expected[0]);
    TestStatement<itmoscript::ast::ContinueStatement>(block_statements[1], expected[1]);
    TestStatement<itmoscript::ast::BreakStatement>(block_statements[2], expected[2]);
}

TEST(ParserLoopTestSuite, ForSimpleTest) {
    std::string code = R"(
        for i in array
            i = i + 1
            continue
            break
        end for
    )";

    auto program = GetParsedProgram(code);
    auto* for_stmt = GetForStmt(program);
    TestIdentifier(for_stmt->range, "array");

    std::vector<std::string> expected = {
        "i = (i + 1)", "continue", "break"
    };

    const auto& block_statements = for_stmt->body->GetStatements();
    ASSERT_EQ(block_statements.size(), expected.size());

    TestStatement<itmoscript::ast::AssignStatement>(block_statements[0], expected[0]);
    TestStatement<itmoscript::ast::ContinueStatement>(block_statements[1], expected[1]);
    TestStatement<itmoscript::ast::BreakStatement>(block_statements[2], expected[2]);
}

TEST(ParserLoopTestSuite, EmptyForTest) {
    std::string code = R"(
        for i in array
        end for
    )";

    auto program = GetParsedProgram(code);
    auto* for_stmt = GetForStmt(program);
    TestIdentifier(for_stmt->range, "array");

    const auto& block_statements = for_stmt->body->GetStatements();
    ASSERT_EQ(block_statements.size(), 0);
}

TEST(ParserLoopTestSuite, EmptyWhileTest) {
    std::string code = R"(
        while true
        end while
    )";
    
    auto program = GetParsedProgram(code);
    auto* while_stmt = GetWhileStmt(program);
    TestLiteralExpression(while_stmt->condition, true);

    const auto& block_statements = while_stmt->body->GetStatements();
    ASSERT_EQ(block_statements.size(), 0);
}
