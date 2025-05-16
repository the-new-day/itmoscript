#include "lexer_test.hpp"

TEST(LexerTestSuite, ReadIntAndFloat) {
    std::string code = "123 9.1 33.091234 900";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kInt, "123"},
        {TT::kFloat, "9.1"},
        {TT::kFloat, "33.091234"},
        {TT::kInt, "900"}
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, ReadStringLiteral) {
    std::string code = R"("Hello World" "biba'boba" "123456")";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kStringLiteral, "\"Hello World\""},
        {TT::kStringLiteral, "\"biba'boba\""},
        {TT::kStringLiteral, "\"123456\""},
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}
