#include "evaluation_units_test.hpp"

TEST(EvaluationLoopTestSuite, WhileLoopSimpleTest) {
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

TEST(EvaluationLoopTestSuite, WhileBreakTest) {
    std::vector<std::pair<std::string, int64_t>> expressions = {
        {R"(
            x = 0
            while x < 10
                x = x + 1
                if x == 5 then
                    break
                end if
            end while
            x    
        )", 5},
        {R"(
            x = 10
            while x > 0
                x = x - 1
                if (x % 2 == 0) then
                    break
                end if
            end while
            x    
        )", 8},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}

TEST(EvaluationLoopTestSuite, WhileContinueTest) {
    std::vector<std::pair<std::string, int64_t>> expressions = {
        {R"(
            x = 0
            even_sum = 0
            while x <= 10
                x = x + 1
                if x % 2 == 1 then
                    continue
                end if
                even_sum = even_sum + x
            end while
            even_sum
        )", 2 + 4 + 6 + 8 + 10},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}
