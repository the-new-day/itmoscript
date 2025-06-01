#include "language_test.hpp"

TEST(BranchTestSuite, SimpleIfTest) {
    std::string code = R"(
        cond = true
        if cond then
            print("true")
        end if
    )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchTestSuite, SimpleElseIfTest) {
    std::string code = R"(
        cond = false
        if cond then
            print("true")
        else
            print("false")
        end if
    )";

    std::string expected = "false";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchTestSuite, ComplexIfTest) {
    std::string code = R"(
        v = 100 * 2 + 10 * 3 + 9
        if v == 30 then
            print(30)
        elseif v == 366 then
            print(366)
        elseif v == 239 then
            print(239)
        else
            print(0)
        end if
    )";

    std::string expected = "239";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchTestSuite, OneLineIfTest) {
    std::string code = "if 2 * 2 == 4 then print(\"2 * 2 == 4\") else print(\"omg\") end if";
    std::string expected = "2 * 2 == 4";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
