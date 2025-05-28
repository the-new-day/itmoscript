#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, FunctionParametersTest) {
    using ID = itmoscript::ast::Identifier;

    auto a = ID{}; a.name = "a";
    auto b = ID{}; b.name = "b";
    auto c = ID{}; c.name = "c";

    std::vector<std::pair<std::string, std::vector<ID>>> expressions = {
        {"function(a, b, c) end function", {a, b, c}},
        {"function(a) end function", {a}},
        {"function() end function", {}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        const auto& params = evaluated.Get<itmoscript::Function>()->parameters;

        ASSERT_EQ(expected.size(), params->size());
        for (size_t i = 0; i < params->size(); ++i) {
            ASSERT_EQ(expected[i].name, params->at(i)->name) << i;
        }
    }
}

TEST(EvaluationTestSuite, FunctionCallSimpleTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"add = function(x, y) return x + y end function \n add(10, 15)", IsValue{25}},
        {"add = function(x, y) return x + y end function \n add(10, 15)", IsValue{25}},
        {"add = function(x, y) return x + y end function \n add(10, 15)", IsValue{25}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected) << "input: " << input;
    }
}

TEST(EvaluationTestSuite, FunctionGlobalScopeTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"x = 10 \n add = function(x, y) return x + y end function \n add(-15, 15)", IsValue{0}},
        {"x = 10 add = function(y) return x + y end function \n add(15)", IsValue{25}},
        {"x = 10 getX = function() return x end function \n getX()", IsValue{10}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected) << "input: " << input;
    }
}

TEST(EvaluationTestSuite, FunctionInnerScopeTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(
            add = function(x, y)
                z = 10000
                add2 = function(x, z)
                    return x + z
                end function

                return add2(x, y)
            end function

            add(1, 2)
        )", IsValue{3}}
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected) << "input: " << input;
    }
}

TEST(EvaluationTestSuite, FunctionShadowingScopeTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(
            x = 10
            get = function(x)
                return x
            end function
            get(5)
            x    
        )", IsValue{10}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected) << "input: " << input;
    }
}
