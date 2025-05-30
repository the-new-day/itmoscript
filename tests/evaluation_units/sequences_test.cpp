#include "evaluation_units_test.hpp"

TEST(EvaluationListsTestSuite, ListIndexingTest) {
    std::vector<std::pair<std::string, int64_t>> expressions{
        {R"(
            x = [1, 2, 3, 4]
            x[2]
        )", 3},
        {R"(
            x = [1]
            x[0] + x[-1]
        )", 2},
        {R"(
            x = [1, 2, 3, 4]
            x[-1]
        )", 4},
        {R"(
            x = [1, 2, 3, 4]
            x[-2]
        )", 3},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::Int>(evaluated, expected);
    }
}

TEST(EvaluationListsTestSuite, ListSlicingTest) {
    std::vector<std::pair<std::string, std::vector<itmoscript::Value>>> expressions{
        {R"(
            x = [1, 2, 3, 4]
            x[:]
        )", {1, 2, 3, 4}},
        {R"(
            x = [1, 2, 3, 4]
            x[1:2]
        )", {2}},
        {R"(
            x = [1, 2, 3, 4]
            x[4:2]
        )", {}},
        {R"(
            x = [1, 2, 3, 4]
            x[:2]
        )", {1, 2}},
        {R"(
            x = [1, 2, 3, 4]
            x[1:]
        )", {2, 3, 4}},
        {R"(
            x = [1, 2, 3, 4]
            x[1000:]
        )", {}},
        {R"(
            x = [1, 2, 3, 4]
            x[:1000]
        )", {1, 2, 3, 4}},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestHeavyValue<itmoscript::List>(evaluated, CreateList(expected));
    }
}

TEST(EvaluationListsTestSuite, StringIndexingTest) {
    std::vector<std::pair<std::string, std::string>> expressions{
        {R"(
            x = "01234"
            x[1]
        )", "1"},
        {R"(
            x = "01234"
            x[0]
        )", "0"},
        {R"(
            x = "01234"
            x[-1]
        )", "4"},
        {R"(
            x = "01234"
            x[-2]
        )", "3"},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestHeavyValue<itmoscript::String>(evaluated, itmoscript::CreateString(expected));
    }
}

TEST(EvaluationListsTestSuite, StringSlicingTest) {
    std::vector<std::pair<std::string, std::string>> expressions{
        {R"(
            x = "012345"
            x[:]
        )", "012345"},
        {R"(
            x = "012345"
            x[1:3]
        )", "12"},
        {R"(
            x = "012345"
            x[0:4]
        )", "0123"},
        {R"(
            x = "012345"
            x[5:4]
        )", ""},
        {R"(
            x = "012345"
            x[:4]
        )", "0123"},
        {R"(
            x = "012345"
            x[2:]
        )", "2345"},
    };

    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestHeavyValue<itmoscript::String>(evaluated, itmoscript::CreateString(expected));
    }
}
