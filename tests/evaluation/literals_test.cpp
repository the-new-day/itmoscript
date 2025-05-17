#include "evaluation_test.hpp"

TEST(EvaluationTestSuite, EvalIntegerExpression) {
    // <input, expected>
    std::vector<std::pair<std::string, int64_t>> expressions{
        {"5", 5},
        {"10", 10},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestInteger(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, EvalBoolExpression) {
    // <input, expected>
    std::vector<std::pair<std::string, bool>> expressions{
        {"true", true},
        {"false", false},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestBool(evaluated, expected);
    }
}
