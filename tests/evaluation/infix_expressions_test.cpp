#include "evaluation_test.hpp"

TEST(EvaluationTestSuite, InfixIntegerExpressionTest) {
    // <input, expected>
    std::vector<std::pair<std::string, int64_t>> expressions{
        {"5 + 5", 10},
        {"10 * 2", 20},
        {"-5 + 5", 0},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
        {"5 % 5", 0},
        {"1244 % 5", 1244 % 5},
        {"1244 ^ 3", 1244 * 1244 * 1244},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::Int>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, InfixBoolExpressionTest) {
    std::vector<std::pair<std::string, bool>> expressions{
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"1 >= 2", false},
        {"1 <= 2", true},
        {"1 >= 1", true},
        {"1 <= 1", true},
        
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::Bool>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, InfixFloatExpressionTest) {
    // <input, expected>
    std::vector<std::pair<std::string, double>> expressions{
        {"5 + 5.5", 10.5},
        {"10 * 2.5", 25.0},
        {"-5 + 5.0", 0},
        {"4^0.5", 2.0},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::Float>(evaluated, expected);
    }
}
