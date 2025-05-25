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
