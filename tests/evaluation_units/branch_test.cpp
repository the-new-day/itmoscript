#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, IfTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"if true then 10 end if", IsValue{10}},
        {"if false then 10 end if", IsValue{itmoscript::NullType{}}},
        {"if 1 then 10 end if", IsValue{10}},
        {"if 0 then 10 end if", IsValue{itmoscript::NullType{}}},
        {"if 1 > 2 then 10 end if", IsValue{10}},
        {"if 1 < 2 then 10 end if", IsValue{itmoscript::NullType{}}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, IfElseTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"if false then 10 else 5 end if", IsValue{5}},
        {"if true then 10 else 5 end if", IsValue{10}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, IfElseIfElseTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(
            if 1 == 2 then
                10
            else if 2 == 2 then
                5
            else
                15
            end if
        )", IsValue{5}},
        {R"(
            if 1 == 2 then
                10
            else if 3 == 2 then
                5
            else
                15
            end if
        )", IsValue{15}},
        {R"(
            if 2 == 2 then
                10
            else if 3 == 2 then
                5
            else
                15
            end if
        )", IsValue{10}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}
