#include "evaluation_units_test.hpp"

TEST(EvaluationAssignmentTestSuite, IntOperatorAssignmentTest) {
    std::vector<std::pair<std::string, int64_t>> expressions{
        {R"(
            x = 5
            x += 10
            x    
        )", 15},
        {R"(
            x = 5
            x -= 10
            x    
        )", -5},
        {R"(
            x = 5
            x *= 10
            x    
        )", 50},
        {R"(
            x = 50
            x /= 10
            x    
        )", 5},
        {R"(
            x = 15
            x %= 10
            x    
        )", 5},
        {R"(
            x = 10
            x ^= 3
            x
        )", 1000},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}

TEST(EvaluationAssignmentTestSuite, StringOperatorAssignmentTest) {
    std::vector<std::pair<std::string, std::string>> expressions{
        {R"(
            x = "aaa"
            x += "bbb"
            x    
        )", "aaabbb"},
        {R"(
            x = ""
            x += "aaa"
            x
        )", "aaa"},
        {R"(
            x = "aaabbb"
            x -= "aaa"
            x
        )", "aaabbb"},
        {R"(
            x = "aaabbb"
            x -= "bbb"
            x
        )", "aaa"},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestHeavyValue<itmoscript::String>(evaluated, itmoscript::CreateString(expected));
    }
}
