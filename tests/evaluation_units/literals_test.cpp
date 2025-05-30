#include "evaluation_units_test.hpp"

TEST(EvaluationLiteralsTestSuite, IntegerLiteralTest) {
    std::vector<std::pair<std::string, int64_t>> expressions{
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}

TEST(EvaluationLiteralsTestSuite, BoolLiteralTest) {
    std::vector<std::pair<std::string, bool>> expressions{
        {"true", true},
        {"false", false},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Bool>(evaluated, expected);
    }
}

TEST(EvaluationLiteralsTestSuite, NullTypeLiteralTest) {
    std::vector<std::string> expressions{
        "nil",
    };

    for (const auto& input : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::NullType>(evaluated, itmoscript::NullType{});
    }
}

TEST(EvaluationLiteralsTestSuite, FloatLiteralTest) {
    std::vector<std::pair<std::string, double>> expressions{
        {"0.999", 0.999},
        {"3.14", 3.14},
        {"100.500", 100.5},
        {"100.50023123", 100.50023123},
        {"-0.999", -0.999},
        {"-3.14", -3.14},
        {"-100.500", -100.5},
        {"-100.50023123", -100.50023123},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Float>(evaluated, expected);
    }
}

TEST(EvaluationLiteralsTestSuite, StringLiteralTest) {
    std::vector<std::pair<std::string, std::string>> expressions{
        {R"("Hello World!")", "Hello World!"},
        {R"("abcdef")", "abcdef"},
        {R"("abc\ndef")", "abc\ndef"},
        {R"("\t")", "\t"},
        {R"("\n\t\n\t")", "\n\t\n\t"},
        {R"("123456")", "123456"},
        {R"("<=>")", "<=>"},
        {R"("####")", "####"},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestHeavyValue<itmoscript::String>(evaluated, itmoscript::CreateString(expected));
    }
}
