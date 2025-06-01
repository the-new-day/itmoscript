#include "stdlib_test.hpp"

using IsValue = itmoscript::Value;

TEST(StdStringTestSuite, LenTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(len("123"))", 3},
        {R"(len(""))", 0},
        {R"(len("1"))", 1},
        {R"(len("1 2 3 4"))", 7},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdStringTestSuite, LowerTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(lower("HELLO"))", IsValue{itmoscript::CreateString("hello")}},
        {R"(lower("###aAa$!@"))", IsValue{itmoscript::CreateString("###aaa$!@")}},
        {R"(lower("123A"))", IsValue{itmoscript::CreateString("123a")}},
        {R"(lower(""))", IsValue{itmoscript::CreateString("")}}
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdStringTestSuite, UpperTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(upper("hello"))", IsValue{itmoscript::CreateString("HELLO")}},
        {R"(upper("###aAa$!@"))", IsValue{itmoscript::CreateString("###AAA$!@")}},
        {R"(upper("123a"))", IsValue{itmoscript::CreateString("123A")}},
        {R"(upper(""))", IsValue{itmoscript::CreateString("")}}
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdStringTestSuite, SplitTest) {
    std::vector<std::pair<std::string, std::vector<std::string>>> test_cases = {
        {R"(split("a,b,c", ","))", {"a", "b", "c"}},
        {R"(split("one two  three", " "))", {"one", "two", "", "three"}},
        {R"(split("nodelim", "-"))", {"nodelim"}},
        {R"(split("", ","))", {""}}
    };

    for (const auto& [input, expected_strs] : test_cases) {
        std::vector<IsValue> expected_values;
        for (const auto& s : expected_strs) {
            expected_values.emplace_back(itmoscript::CreateString(s));
        }
        IsValue expected = itmoscript::CreateList(expected_values);

        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdStringTestSuite, JoinTest) {
    std::vector<std::pair<std::string, std::string>> expressions = {
        {R"(join(["a", "b", "c"], ","))", "a,b,c"},
        {R"(join(["one", "", "three"], " "))", "one  three"},
        {R"(join([], "-"))", ""},
        {R"(join(["single"], ""))", "single"}
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_TRUE(evaluated.GetType() == itmoscript::ValueType::kString);
        const std::string& got = *evaluated.Get<itmoscript::String>();

        ASSERT_EQ(expected, got);
    }
}

TEST(StdStringTestSuite, ReplaceTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(replace("hello world", "world", "there"))", IsValue{itmoscript::CreateString("hello there")}},
        {R"(replace("aabbcc", "bb", "x"))", IsValue{itmoscript::CreateString("aaxcc")}},
        {R"(replace("no match", "xyz", "123"))", IsValue{itmoscript::CreateString("no match")}},
        {R"(replace("", "x", "y"))", IsValue{itmoscript::CreateString("")}},
        {R"(replace("aaa", "a", "aa"))", IsValue{itmoscript::CreateString("aaaaaa")}}
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}
