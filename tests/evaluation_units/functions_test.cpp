#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, FunctionParametersTest) {
    using ID = itmoscript::Identifier;

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
            ASSERT_EQ(expected[i].name, params->at(i).name) << i;
        }
    }
}
