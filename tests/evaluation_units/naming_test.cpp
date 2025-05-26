#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, VariableNamingTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"x = 10 x", IsValue{10}},
        {"x = nil x", IsValue{itmoscript::NullType{}}},
        {"x = 10.5 x", IsValue{10.5}},
        {"x = \"hello\" x", IsValue{itmoscript::String{"hello"}}},
        {"x = true x", IsValue{true}},

        {"x = \"aaa\" + \"bbb\" x", IsValue{itmoscript::String{"aaabbb"}}},

        {"x = 10 y = 5 z = x + y z", IsValue{15}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, ValueChangeTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"x = 5 x = 10 x", IsValue{10}},
        {"x = 100 x = nil x", IsValue{itmoscript::NullType{}}},
        {"x = true x = 10.5 x", IsValue{10.5}},
        {"x = function() end function x = \"hello\" x", IsValue{itmoscript::String{"hello"}}},
        {"x = 9 + 9 x = true x", IsValue{true}},

        {"x = 0 x = \"aaa\" + \"bbb\" x", IsValue{itmoscript::String{"aaabbb"}}},

        {"x = 10 y = 5 z = x + y z = 500 z", IsValue{500}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}
