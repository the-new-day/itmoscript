#include "evaluation_test.hpp"

TEST(EvaluationTestSuite, StringAdditionTest) {
    std::vector<std::pair<std::string, std::string>> expressions{
        {R"("hello" + "world")", "helloworld"},
        {R"("\n" + "\n")", "\n\n"},
        {R"("" + "")", ""},
        {R"("" + "abcabc")", "abcabc"},
    };

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::String>(evaluated, expected);
    }
}

TEST(EvaluationTestSuite, StringMultiplicationTest) {
    std::vector<std::pair<std::string, std::string>> expressions{
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

    for (const auto& [input, expected] : expressions) {
        Value evaluated = Eval(input);
        TestValue<ItmoScript::String>(evaluated, expected);
    }
}
