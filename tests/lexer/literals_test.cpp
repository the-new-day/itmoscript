#include "lexer_test.hpp"

TEST(LexerTestSuite, ReadIntAndFloat) {
    std::string code = "123 9.1 33.091234 900";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kInt, "123"},
        {TT::kFloat, "9.1"},
        {TT::kFloat, "33.091234"},
        {TT::kInt, "900"}
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, ReadStringLiteral) {
    std::string code = R"("Hello World" "biba'boba" "123456" "\"" "\\")";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kStringLiteral, R"("Hello World")"},
        {TT::kStringLiteral, R"("biba'boba")"},
        {TT::kStringLiteral, R"("123456")"},
        {TT::kStringLiteral, R"("\"")"},
        {TT::kStringLiteral, R"("\\")"},
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, ReadScientificNotationFloat) {
    std::string code = "1e3 1.5e2 -2.1E-1 5e+0";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kFloat, "1e3"},
        {TT::kFloat, "1.5e2"},
        {TT::kMinus, "-"},
        {TT::kFloat, "2.1E-1"},
        {TT::kFloat, "5e+0"}
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}
