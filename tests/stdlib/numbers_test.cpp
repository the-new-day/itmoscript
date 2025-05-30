#include "stdlib_test.hpp"

using IsValue = itmoscript::Value;

TEST(StdNumbersTestSuite, AbsTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"abs(3)", 3},
        {"abs(-1)", 1},
        {"abs(0)", 0},
        {"abs(-21423423)", 21423423},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

#include "stdlib_test.hpp"

using IsValue = itmoscript::Value;

TEST(StdNumbersTestSuite, CeilTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"ceil(3.2)", 4.0},
        {"ceil(-1.7)", -1.0},
        {"ceil(0.0)", 0.0},
        {"ceil(5.999)", 6.0},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdNumbersTestSuite, FloorTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"floor(3.7)", 3.0},
        {"floor(-1.2)", -2.0},
        {"floor(0.0)", 0.0},
        {"floor(5.999)", 5.0},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdNumbersTestSuite, RoundTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"round(3.2)", 3.0},
        {"round(3.5)", 4.0},
        {"round(-1.4)", -1.0},
        {"round(-1.6)", -2.0},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdNumbersTestSuite, SqrtTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"sqrt(4)", 2.0},
        {"sqrt(9)", 3.0},
        {"sqrt(0)", 0.0},
        {"sqrt(2.25)", 1.5},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdNumbersTestSuite, RndTest) {
    std::vector<std::tuple<std::string, int, int>> test_cases = {
        {"rnd(1)", 0, 0},
        {"rnd(10)", 0, 9},
        {"rnd(100)", 0, 99},
    };

    for (const auto& [input, min_val, max_val] : test_cases) {
        IsValue evaluated = Eval(input);
        itmoscript::Int value = evaluated.Get<itmoscript::Int>();

        ASSERT_EQ(value, value);
        ASSERT_GE(value, min_val);
        ASSERT_LE(value, max_val);
    }
}

TEST(StdNumbersTestSuite, ParseNumTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"parse_num(\"123\")", 123},
        {"parse_num(\"-45.67\")", -45.67},
        {"parse_num(\"0\")", 0},
        {"parse_num(\"not_a_number\")", IsValue{itmoscript::NullType{}}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdNumbersTestSuite, ToStringTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {"to_string(123)", IsValue{itmoscript::CreateString("123")}},
        {"to_string(-45.67)", IsValue{itmoscript::CreateString(std::to_string(-45.67))}},
        {"to_string(0)", IsValue{itmoscript::CreateString("0")}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}
