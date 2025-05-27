#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, GlobalScopeTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"x = 10 \n getX = function() x end function \n x = 25 \n getX()", IsValue{25}},
        {R"(
            x = 10
            getX = function()
                get = function()
                    return x
                end function
                get()
            end function
            y = getX()
            x = 100
            y + getX()
        )", IsValue{110}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected) << "input: " << input;
    }
}
