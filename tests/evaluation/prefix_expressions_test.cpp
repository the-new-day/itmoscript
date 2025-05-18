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
        TestBool(evaluated, expected);
    }
}
