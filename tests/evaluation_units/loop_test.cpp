#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, WhileLoopSimpleTest) {
    std::vector<std::pair<std::string, int64_t>> expressions = {
        {R"(
            x = 0
            while x < 10
                x = x + 1
            end while
            x    
        )", 10},
        {R"(
            x = 10
            while x > 0
                x = x - 1
            end while
            x    
        )", 0},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}
