#include "parser_test.hpp"

TEST(ParserStatementsTestSuite, VariableAssignmentSimpleTest) {
    std::string code = R"(
        x = 5
        y = 10
        foobar = 100 + 500
        z = x
        a = b + c * 3
    )";

    std::vector<std::string> expected_identifiers = {
        "x",
        "y",
        "foobar",
        "z",
        "a",
    };

    std::vector<std::string> expected_expressions = {
        "5",
        "10",
        "(100 + 500)",
        "x",
        "(b + (c * 3))",
    };

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();

    ASSERT_EQ(statements.size(), expected_identifiers.size());

    for (size_t i = 0; i < statements.size(); ++i) {
        auto* assign_stmt = dynamic_cast<itmoscript::ast::AssignStatement*>(statements[i].get());
        ASSERT_NE(assign_stmt, nullptr) << "Statement " << i << " is not an AssignStatement";

        ASSERT_EQ(assign_stmt->ident->name, expected_identifiers[i]);
        ASSERT_EQ(assign_stmt->expr->String(), expected_expressions[i]);
    }
}

TEST(ParserStatementsTestSuite, ReturnSimpleTest) {
    std::string code = R"(
        return 5
        return 10
        return x
        return
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();

    ASSERT_EQ(statements.size(), 4);

    std::vector<std::string> expected_exprs = {"5", "10", "x", ""};

    for (size_t i = 0; i < statements.size(); ++i) {
        auto* return_stmt = dynamic_cast<itmoscript::ast::ReturnStatement*>(statements[i].get());
        ASSERT_NE(return_stmt, nullptr) << "Statement " << i << " is not a ReturnStatement";

        if (expected_exprs[i].empty()) {
            ASSERT_EQ(return_stmt->expr, nullptr);
        } else {
            ASSERT_EQ(return_stmt->expr->String(), expected_exprs[i]);
        }
    }
}
