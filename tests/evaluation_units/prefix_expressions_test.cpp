#include "evaluation_units_test.hpp"

TEST(EvaluationTestSuite, BangOperatorTest) {
    // <input, expected>
    std::vector<std::pair<std::string, bool>> expressions{
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!0", true},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
        {"!!0", false},
        {"!!5.3", true},
        {"!!0.0", false},
        {"!\"\"", true},
        {"!\"aaa\"", false},
        {"!!\"\"", false},
        {"!!\"aaa\"", true},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Bool>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, UnaryMinusOperatorTest) {
    std::vector<std::pair<std::string, int64_t>> int_expressions{
        {"-30", -30},
        {"-(-30)", 30},
    };

    for (const auto& [input, expected] : int_expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }

    std::vector<std::pair<std::string, double>> float_expressions{
        {"-30.3", -30.3},
        {"-(-30.3)", 30.3},
    };

    for (const auto& [input, expected] : float_expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Float>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, UnaryPlusOperatorTest) {
    std::vector<std::pair<std::string, int64_t>> int_expressions{
        {"+30", +30},
        {"+(-30)", -30},
    };

    for (const auto& [input, expected] : int_expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }

    std::vector<std::pair<std::string, double>> float_expressions{
        {"+30.3", +30.3},
        {"+(-30.3)", -30.3},
    };

    for (const auto& [input, expected] : float_expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Float>(evaluated, expected);
    }
}
