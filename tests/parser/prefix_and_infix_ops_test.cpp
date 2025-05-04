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

        auto* integer_literal = dynamic_cast<ItmoScript::IntegerLiteral*>(prefix_expr->right.get());
        ASSERT_NE(integer_literal, nullptr);

        int64_t int_value = integer_literal->value;
        ASSERT_EQ(int_value, test_expr.value);
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

        auto* infix_expr = dynamic_cast<ItmoScript::InfixExpression*>(expr_stmt->expr.get());
        ASSERT_NE(infix_expr, nullptr);
        ASSERT_NE(infix_expr->right, nullptr);
        ASSERT_NE(infix_expr->left, nullptr);

        ASSERT_EQ(infix_expr->oper, test_expr.oper);

        auto* left_integer_literal = dynamic_cast<ItmoScript::IntegerLiteral*>(infix_expr->left.get());
        ASSERT_NE(left_integer_literal, nullptr);

        auto* right_integer_literal = dynamic_cast<ItmoScript::IntegerLiteral*>(infix_expr->right.get());
        ASSERT_NE(right_integer_literal, nullptr);

        ASSERT_EQ(right_integer_literal->value, test_expr.right_value);
        ASSERT_EQ(left_integer_literal->value, test_expr.left_value);
    }
}

TEST(ParserTestSuite, OperatorPrecedenceParsingTest) {
    // <input, expected>
    std::vector<std::pair<std::string, std::string>> tests{
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
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
