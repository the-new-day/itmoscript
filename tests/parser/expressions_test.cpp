#include "parser_test.hpp"

TEST(ParserExpressionsTestSuite, ExpressionStringTest) {
    std::string code = R"(
        x1 = x2
    )";

    auto program = GetParsedProgram(code);
    ASSERT_EQ(program.String(), "x1 = x2");
}

TEST(ParserExpressionsTestSuite, IdentifierExpressionStringTest) {
    std::string code = R"(
        foobar
    )";

    auto program = GetParsedProgram(code);
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);

    std::string ident = expr_stmt->String();
    ASSERT_EQ(ident, "foobar");
}

TEST(ParserExpressionsTestSuite, IntPrefixOperatorsTest) {
    std::vector<PrefixOpExpr<int64_t>> expressions{
        {"!5", itmoscript::TokenType::kBang, 5},
        {"!50234", itmoscript::TokenType::kBang, 50234},
        {"!33", itmoscript::TokenType::kBang, 33},
        {"-15", itmoscript::TokenType::kMinus, 15},
        {"-0", itmoscript::TokenType::kMinus, 0},
        {"-22223333", itmoscript::TokenType::kMinus, 22223333},
        {"+22223333", itmoscript::TokenType::kPlus, 22223333},
        {"+0", itmoscript::TokenType::kPlus, 0},
    };

    TestPrefixLiteralsExpressions<int64_t>(expressions);
}

TEST(ParserExpressionsTestSuite, BooleanPrefixOperatorsTest) {
    std::vector<PrefixOpExpr<bool>> expressions{
        {"!true", itmoscript::TokenType::kBang, true},
        {"!false", itmoscript::TokenType::kBang, false},
        {"not true", itmoscript::TokenType::kNot, true},
        {"not false", itmoscript::TokenType::kNot, false},
    };

    TestPrefixLiteralsExpressions<bool>(expressions);
}

TEST(ParserExpressionsTestSuite, IntInfixOperatorsTest) {
    std::vector<InfixOpExpr<int64_t>> expressions{
        {"5 + 10", itmoscript::TokenType::kPlus, 5, 10},
        {"10 - 5", itmoscript::TokenType::kMinus, 10, 5},
        {"5 + 5", itmoscript::TokenType::kPlus, 5, 5},
        {"5 - 5", itmoscript::TokenType::kMinus, 5, 5},
        {"5 * 5", itmoscript::TokenType::kAsterisk, 5, 5},
        {"5 / 5", itmoscript::TokenType::kSlash, 5, 5},
        {"5 % 5", itmoscript::TokenType::kPercent, 5, 5},
        {"5 ^ 5", itmoscript::TokenType::kPow, 5, 5},
        {"5 > 5", itmoscript::TokenType::kGreater, 5, 5},
        {"5 < 5", itmoscript::TokenType::kLess, 5, 5},
        {"5 == 5", itmoscript::TokenType::kEqual, 5, 5},
        {"5 != 5", itmoscript::TokenType::kNotEqual, 5, 5},
        {"5 >= 5", itmoscript::TokenType::kGreaterOrEqual, 5, 5},
        {"5 <= 5", itmoscript::TokenType::kLessOrEqual, 5, 5},
    };

    TestInfixLiteralsExpressions<int64_t>(expressions);
}

TEST(ParserExpressionsTestSuite, BooleanInfixOperatorsTest) {
    std::vector<InfixOpExpr<bool>> expressions{
        {"true == false", itmoscript::TokenType::kEqual, true, false},
        {"true == true", itmoscript::TokenType::kEqual, true, true},
        {"true != false", itmoscript::TokenType::kNotEqual, true, false},
        {"true and false", itmoscript::TokenType::kAnd, true, false},
        {"true or false", itmoscript::TokenType::kOr, true, false},
    };
    
    TestInfixLiteralsExpressions<bool>(expressions);
}

TEST(ParserExpressionsTestSuite, OperatorPrecedenceParsingTest) {
    std::vector<std::pair<std::string, std::string>> tests{
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
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

        // grouped expressions
        {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == false)", "(!(true == false))"},
        {"(3 * (2 - 7) / x == 5 / (1 + 1) - (9 + 0))", "(((3 * (2 - 7)) / x) == ((5 / (1 + 1)) - (9 + 0)))"},

        // calls
        {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
        {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
        {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
    };

    for (const auto& [input, expected] : tests) {
        auto program = GetParsedProgram(input);
        ASSERT_EQ(program.String(), expected);
    }
}

TEST(ParserExpressionsTestSuite, IndexOperatorTest) {
    // <code, operand, is_slice, indeces (empty == no index)>
    std::vector<std::tuple<std::string, std::string, bool, std::pair<std::string, std::string>>> tests{
        {"x[1]", "x", false, {"1", ""}},
        {"x[1:2]", "x", true, {"1", "2"}},
        {"x[:2]", "x", true, {"", "2"}},
        {"x[:]", "x", true, {"", ""}},
    };

    for (const auto& [input, operand, is_slice, indeces] : tests) {
        auto program = GetParsedProgram(input);
        const auto& statements = program.GetStatements();

        ASSERT_EQ(program.GetStatements().size(), 1) << input;

        auto* expr_stmt = dynamic_cast<itmoscript::ast::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        auto* expr = dynamic_cast<itmoscript::ast::IndexOperatorExpression*>(expr_stmt->expr.get());
        ASSERT_NE(expr, nullptr);
        ASSERT_NE(expr->operand, nullptr);

        ASSERT_EQ(expr->operand->String(), operand);
        
        if (is_slice) {
            if (indeces.first.empty()) {
                ASSERT_EQ(expr->index, nullptr);
            } else {
                ASSERT_NE(expr->index, nullptr);
                ASSERT_EQ(expr->index->String(), indeces.first);
            }
            
            if (indeces.second.empty()) {
                ASSERT_EQ(expr->second_index, nullptr);
            } else {
                ASSERT_NE(expr->second_index, nullptr);
                ASSERT_EQ(expr->second_index->String(), indeces.second);
            }
        }
    }
}
