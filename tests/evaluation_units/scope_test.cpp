#include "evaluation_units_test.hpp"

#include "evaluation/exceptions/UndefinedNameError.hpp"

TEST(EvaluationTestSuite, FunctionNestedScopeToGlobalTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"x = 10 \n getX = function() return x end function \n x = 25 \n getX()", IsValue{25}},
        {R"(
            x = 10
            getX = function()
                get = function()
                    return x
                end function
                return get()
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

TEST(EvaluationTestSuite, IfBlockScopeSimpleTest) {
    std::string expr = R"(
        x = 10
        if x == 0 then
            y = 10
        end if
        y
    )";

    ASSERT_THROW(Eval(expr), itmoscript::lang_exceptions::UndefinedNameError);
}
