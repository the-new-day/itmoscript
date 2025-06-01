#include "stdlib_test.hpp"

using IsValue = itmoscript::Value;

TEST(StdListTestSuite, RangeTest) {
    std::vector<std::pair<std::string, std::vector<itmoscript::Int>>> test_cases = {
        {R"(range(5))", {0, 1, 2, 3, 4}},
        {R"(range(1, 5))", {1, 2, 3, 4}},
        {R"(range(5, 1, -1))", {5, 4, 3, 2}},
        {R"(range(1, 10, 3))", {1, 4, 7}},
        {R"(range(0, 5, 2))", {0, 2, 4}},
        {R"(range(5, 5, 1))", {}},
        {R"(range(0, 3))", {0, 1, 2}},
        {R"(range(10, 0, -3))", {10, 7, 4, 1}}
    };

    for (const auto& [input, expected_nums] : test_cases) {
        std::vector<IsValue> expected_values;
        for (itmoscript::Int num : expected_nums) {
            expected_values.emplace_back(num);
        }

        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}

TEST(StdListTestSuite, LenTest) {
    std::vector<std::pair<std::string, IsValue>> expressions = {
        {R"(len([1, 2, 3]))", 3},
        {R"(len([]))", 0},
        {R"(len("hello"))", 5},
        {R"(len(""))", 0}
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, expected);
    }
}

TEST(StdListTestSuite, PushTest) {
    std::vector<std::pair<std::string, std::vector<IsValue>>> test_cases = {
        {R"(push([1, 2], 3))", {1, 2, 3}},
        {R"(push([], "a"))", {itmoscript::CreateString("a")}},
        {R"(push(["x"], 42))", {itmoscript::CreateString("x"), 42}}
    };

    for (const auto& [input, expected_values] : test_cases) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}

TEST(StdListTestSuite, PopTest) {
    // src, popped, remaining
    std::vector<std::pair<std::string, std::vector<IsValue>>> test_cases = {
        {R"(
            x = [1, 2, 3]
            pop(x)
            x
        )", {1, 2}},
        {R"(
            x = ["single"]
            pop(x)
            x
        )", {}},
    };

    for (const auto& [input, expected_values] : test_cases) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}

TEST(StdListTestSuite, InsertTest) {
    std::vector<std::pair<std::string, std::vector<IsValue>>> test_cases = {
        {R"(insert([1, 3], 1, 2))", {1, 2, 3}},
        {R"(insert([1, 3], 0, 2))", {2, 1, 3}},
        {R"(insert([], 0, 4))", {4}},
        {R"(insert([1, 2], 2, 3))", {1, 2, 3}}
    };

    for (const auto& [input, expected_values] : test_cases) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}

TEST(StdListTestSuite, RemoveTest) {
    std::vector<std::pair<std::string, std::vector<IsValue>>> test_cases = {
        {R"(remove([1, 2, 3], 1))", {1, 3}},
        {R"(remove([1, 2, 3], 0))", {2, 3}},
        {R"(remove([true, false], 1))", {IsValue{true}}},
    };

    for (const auto& [input, expected_values] : test_cases) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}

TEST(StdListTestSuite, SortTest) {
    std::vector<std::pair<std::string, std::vector<IsValue>>> test_cases = {
        {R"(lst = [3, 1, 2] sort(lst) lst)", 
         {1, 2, 3}},
        
        {R"(lst = ["b", "a", "c"] sort(lst) lst)",
         {itmoscript::CreateString("a"), itmoscript::CreateString("b"), itmoscript::CreateString("c")}},
         
        {R"(lst = [true, false, true] sort(lst) lst)",
         {IsValue{false}, IsValue{true}, IsValue{true}}},
         
        {R"(lst = [1, "a", 2] sort(lst) lst)", 
         {1, 2, itmoscript::CreateString("a")}},
         
        {R"(lst = [] sort(lst) lst)", {}}
    };

    for (const auto& [input, expected_values] : test_cases) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}

TEST(StdListTestSuite, SetTest) {
    std::vector<std::pair<std::string, std::vector<IsValue>>> test_cases = {
        {R"(set([1, 2, 3], 1, 4))", {1, 4, 3}},
        {R"(set([1], 0, 2))", {2}},
        {R"(set([1, 1], 1, "hello"))", {1, itmoscript::CreateString("hello")}},
    };

    for (const auto& [input, expected_values] : test_cases) {
        IsValue evaluated = Eval(input);
        ASSERT_EQ(evaluated, itmoscript::CreateList(expected_values));
    }
}
