#include "parser_test.hpp"

TEST(ParserTestSuite, IntPrefixOperatorsTest) {
    std::vector<PrefixOpExpr<int64_t>> expressions{
        {"!5", "!", 5},
        {"!50234", "!", 50234},
        {"!33", "!", 33},
        {"-15", "-", 15},
        {"-0", "-", 0},
        {"-22223333", "-", 22223333},
        {"+22223333", "+", 22223333},
        {"+0", "+", 0},
    };

    TestPrefixLiteralsExpressions<int64_t>(expressions);
}

TEST(ParserTestSuite, BooleanPrefixOperatorsTest) {
    std::vector<PrefixOpExpr<bool>> expressions{
        {"!true", "!", true},
        {"!false", "!", false},
    };

    TestPrefixLiteralsExpressions<bool>(expressions);
}

TEST(ParserTestSuite, IntInfixOperatorsTest) {
    std::vector<InfixOpExpr<int64_t>> expressions{
        {"5 + 5", "+", 5, 5},
        {"5 - 5", "-", 5, 5},
        {"5 * 5", "*", 5, 5},
        {"5 / 5", "/", 5, 5},
        {"5 % 5", "%", 5, 5},
        {"5 ^ 5", "^", 5, 5},
        {"5 > 5", ">", 5, 5},
        {"5 < 5", "<", 5, 5},
        {"5 == 5", "==", 5, 5},
        {"5 != 5", "!=", 5, 5},
        {"5 >= 5", ">=", 5, 5},
        {"5 <= 5", "<=", 5, 5},
    };

    TestInfixLiteralsExpressions<int64_t>(expressions);
}

TEST(ParserTestSuite, BooleanInfixOperatorsTest) {
    std::vector<InfixOpExpr<bool>> expressions{
        {"true == false", "==", true, false},
        {"true == true", "==", true, true},
        {"true != false", "!=", true, false},
    };
    
    TestInfixLiteralsExpressions<bool>(expressions);
}

TEST(ParserTestSuite, OperatorPrecedenceParsingTest) {
    // <input, expected>
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
