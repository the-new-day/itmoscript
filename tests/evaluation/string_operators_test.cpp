#include "evaluation_test.hpp"

void TestStringExpressions(const std::vector<std::pair<std::string, std::string>>& expressions) {
    for (const auto& [input, expected] : expressions) {
        IsValue evaluated = Eval(input);
        TestValue<itmoscript::String>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, StringAdditionTest) {
    std::vector<std::pair<std::string, std::string>> expressions = {
        {R"("hello" + "world")", "helloworld"},
        {R"("\n" + "\n")", "\n\n"},
        {R"("" + "")", ""},
        {R"("" + "abcabc")", "abcabc"},
    };
    
    TestStringExpressions(expressions);
}

TEST(EvaluationTestSuite, StringMultiplicationTest) {
    std::vector<std::pair<std::string, std::string>> expressions = {
        {R"("hello" * 2)", "hellohello"},
        {R"("\n" * 4)", "\n\n\n\n"},
        {R"("" * 100500)", ""},
        {R"("" * 0)", ""},
        {R"("aaaaa" * 0)", ""},
        {R"("aa" * 2.5)", "aaaaa"},
        {R"("aa" * 0.5)", "a"},
        {R"("a" * 0.5)", ""},
        {R"("" * 0.5)", ""},
    };
    
    TestStringExpressions(expressions);
}

TEST(EvaluationTestSuite, StringSubstractionsTest) {
    std::vector<std::pair<std::string, std::string>> expressions = {
        {R"("hello" - "llo")", "he"},
        {R"("hello" - "")", "hello"},
        {R"("hello" - "hello")", ""},
        {R"("hello" - "abc")", "hello"},
        {R"("" - "")", ""},
        {R"("" - "hello")", ""},
    };
    
    TestStringExpressions(expressions);
}
