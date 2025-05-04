#include "parser_test.hpp"

struct PrefixOpExpr {
    std::string input;
    std::string oper;
    int64_t value;
};

struct InfixOpExpr {
    std::string input;
    std::string oper;
    int64_t left_value;
    int64_t right_value;
};

TEST(ParserTestSuite, SimplePrefixOperatorsTest) {
    std::vector<PrefixOpExpr> expressions{
        {"!5", "!", 5},
        {"!50234", "!", 50234},
        {"!33", "!", 33},
        {"-15", "-", 15},
        {"-0", "-", 0},
        {"-22223333", "-", 22223333},
    };

    for (const auto& test_expr : expressions) {
        ItmoScript::Lexer lexer{test_expr.input};
        ItmoScript::Parser parser{lexer};
        ItmoScript::Program program = parser.ParseProgram();
        
        PrintParserErrors(parser);

        const auto& statements = program.GetStatements();
        ASSERT_EQ(statements.size(), 1);

        auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        auto* prefix_expr = dynamic_cast<ItmoScript::PrefixExpression*>(expr_stmt->expr.get());
        ASSERT_NE(prefix_expr, nullptr);
        ASSERT_NE(prefix_expr->right, nullptr);

        ASSERT_EQ(prefix_expr->oper, test_expr.oper);

        TestIntegerLiteral(prefix_expr->right, test_expr.value);
    }
}

TEST(ParserTestSuite, SimpleInfixOperatorsTest) {
    std::vector<InfixOpExpr> expressions{
        {"5 + 5", "+", 5, 5},
        {"5 - 5", "-", 5, 5},
        {"5 * 5", "*", 5, 5},
        {"5 / 5", "/", 5, 5},
        {"5 > 5", ">", 5, 5},
        {"5 < 5", "<", 5, 5},
        {"5 == 5", "==", 5, 5},
        {"5 != 5", "!=", 5, 5},
    };

    for (const auto& test_expr : expressions) {
        ItmoScript::Lexer lexer{test_expr.input};
        ItmoScript::Parser parser{lexer};
        ItmoScript::Program program = parser.ParseProgram();
        
        PrintParserErrors(parser);

        const auto& statements = program.GetStatements();
        ASSERT_EQ(statements.size(), 1);

        auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        TestInfixExpression(expr_stmt->expr, test_expr.left_value, test_expr.oper, test_expr.right_value);
    }
}

TEST(ParserTestSuite, OperatorPrecedenceParsingTest) {
    // <input, expected>
    std::vector<std::pair<std::string, std::string>> tests{
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + -c", "(a + (-c))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4\n-5 * 5", "(3 + 4)\n((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
    };

    for (const auto& [input, expected] : tests) {
        ItmoScript::Lexer lexer{input};
        ItmoScript::Parser parser{lexer};
        ItmoScript::Program program = parser.ParseProgram();

        PrintParserErrors(parser);
        ASSERT_EQ(program.String(), expected);
    }
}
