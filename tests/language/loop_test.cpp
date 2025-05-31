#include "language_test.hpp"

TEST(LoopTestSuit, ForLoop) {
    std::string code = R"(
        for i in range(0,5,1)
            print(i)
        end for
    )";

    std::string expected = "01234";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(LoopTestSuit, WhileLoop) {
    std::string code = R"(
        s = "ITMO"
        while  len(s) < 12
            s = s * 2
        end while
        print(s)
    )";

    std::string expected = "\"ITMOITMOITMOITMO\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
