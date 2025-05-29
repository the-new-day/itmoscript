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

TEST(EvaluationLoopTestSuite, ForLoopSimpleTest) {
    std::vector<std::pair<std::string, int64_t>> expressions = {
        {R"(
            arr = [1, 2, 3]
            sum = 0
            for i in arr
                sum += i 
            end for
            sum
        )", 6},
        {R"(
            arr = []
            sum = 0
            for i in arr
                sum += i 
            end for
            sum  
        )", 0},
        {R"(
            arr = [4]
            sum = 0
            for i in arr
                sum += i 
            end for
            sum  
        )", 4},
        {R"(
            sum = 0
            for i in [1, 2, 3]
                sum += i 
            end for
            sum  
        )", 6},
        {R"(
            sum = 0
            for i in []
                sum += i 
            end for
            sum  
        )", 0},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}

TEST(EvaluationLoopTestSuite, ForLoopContinueTest) {
    std::vector<std::pair<std::string, int64_t>> expressions = {
        {R"(
            arr = [1, 2, 3, 4, 5, 6]
            sum = 0
            for i in arr
                continue
                sum += i 
            end for
            sum
        )", 0},
        {R"(
            arr = [1, 2, 3, 4, 5, 6]
            sum = 0
            for i in arr
                if i % 2 == 0 then
                    continue
                end if
                sum += i 
            end for
            sum 
        )", 1 + 3 + 5},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}

TEST(EvaluationLoopTestSuite, ForLoopBreakTest) {
    std::vector<std::pair<std::string, int64_t>> expressions = {
        {R"(
            arr = [1, 2, 3, 4, 5, 6]
            sum = 0
            for i in arr
                sum += i 
                break
            end for
            sum
        )", 1},
        {R"(
            arr = [1, 2, 3, 4, 5, 6]
            sum = 0
            for i in arr
                if i == 4 then
                    break
                end if
                sum += i 
            end for
            sum 
        )", 1 + 2 + 3},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}
