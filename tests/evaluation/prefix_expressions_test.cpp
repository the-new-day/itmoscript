#include "evaluation_test.hpp"

TEST(EvaluationTestSuite, BangOperatorTest) {
    // <input, expected>
    std::vector<std::pair<std::string, bool>> expressions{
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!0", true},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
        {"!!0", false},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::Bool>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, UnaryMinusOperatorTest) {
    // <input, expected>
    std::vector<std::pair<std::string, int64_t>> expressions{
        {"-30", -30},
        {"-(-30)", 30},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::Int>(evaluated, expected);
    }
}
