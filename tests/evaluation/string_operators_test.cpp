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
