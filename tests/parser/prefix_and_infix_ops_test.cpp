#include "parser_test.hpp"

TEST(ParserTestSuite, SimplePrefixOperatorsTest) {
    std::vector<PrefixOpExpr<int64_t>> expressions{
        {"!5", "!", 5},
        {"!50234", "!", 50234},
        {"!33", "!", 33},
        {"-15", "-", 15},
        {"-0", "-", 0},
        {"-22223333", "-", 22223333},
    };

    TestPrefixLiteralsExpressions<int64_t>(expressions);
}

TEST(ParserTestSuite, SimpleInfixOperatorsTest) {
    std::vector<InfixOpExpr<int64_t>> expressions{
        {"5 + 5", "+", 5, 5},
        {"5 - 5", "-", 5, 5},
        {"5 * 5", "*", 5, 5},
        {"5 / 5", "/", 5, 5},
        {"5 > 5", ">", 5, 5},
        {"5 < 5", "<", 5, 5},
        {"5 == 5", "==", 5, 5},
        {"5 != 5", "!=", 5, 5},
    };

    TestInfixLiteralsExpressions<int64_t>(expressions);
}

TEST(ParserTestSuite, InfixOperatorsWithBooleanTest) {
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
    };

    for (const auto& [input, expected] : tests) {
        ItmoScript::Lexer lexer{input};
        ItmoScript::Parser parser{lexer};
        ItmoScript::Program program = parser.ParseProgram();

        PrintParserErrors(parser);
        ASSERT_EQ(program.String(), expected);
    }
}
