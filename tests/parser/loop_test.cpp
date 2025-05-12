#include "parser_test.hpp"

TEST(ParserTestSuite, SimpleWhileTest) {
    std::string code = R"(
        while i < 10
            i = i + 1
            continue
            break
        end while
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* while_stmt = dynamic_cast<ItmoScript::WhileStatement*>(statements[0].get());
    ASSERT_NE(while_stmt, nullptr);
    ASSERT_NE(while_stmt->condition, nullptr);
    ASSERT_NE(while_stmt->body, nullptr);

    TestInfixExpression(while_stmt->condition, "i", "<", 10);

    auto* body_block = dynamic_cast<ItmoScript::BlockStatement*>(while_stmt->body.get());
    ASSERT_NE(body_block, nullptr);

    std::vector<std::string> expected = {
        "i = (i + 1)", "continue", "break"
    };

    const auto& block_statements = body_block->GetStatements();
    ASSERT_EQ(block_statements.size(), expected.size());

    TestStatement<ItmoScript::AssignStatement>(block_statements[0], expected[0]);
    TestStatement<ItmoScript::ContinueStatement>(block_statements[1], expected[1]);
    TestStatement<ItmoScript::BreakStatement>(block_statements[2], expected[2]);
}
