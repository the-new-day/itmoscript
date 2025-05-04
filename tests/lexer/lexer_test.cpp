#include "lexer_test.hpp"

// TODO: add tests for Ln, Col

TEST(LexerTestSuite, OneCharToken) {
    std::string code;
    std::vector<ItmoScript::Token> expected;

    for (const auto& [token, type] : ItmoScript::kOneCharTokens) {
        if (type == TT::kEOF) continue;

        expected.push_back({.type = type, .literal = std::string{token}});
        code += token;
        code += ' ';
    }

    expected.push_back({.type = TT::kEOF});
    ItmoScript::Lexer lexer{code};
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, OneCharIdentiriers) {
    std::string code = "a b c d e f";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kIdentifier, "a"},
        {TT::kIdentifier, "b"},
        {TT::kIdentifier, "c"},
        {TT::kIdentifier, "d"},
        {TT::kIdentifier, "e"},
        {TT::kIdentifier, "f"},
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, IdentifiersAssignment) {
    std::string code = "n = number_2";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kIdentifier, "n"},
        {TT::kAssing, "="},
        {TT::kIdentifier, "number_2"},
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, FunctionCreation) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function
    )";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kIdentifier, "incr"},
        {TT::kAssing, "="},
        {TT::kFunction, "function"},
        {TT::kLParen, "("},
        {TT::kIdentifier, "value"},
        {TT::kRParen, ")"},
        {TT::kReturn, "return"},
        {TT::kIdentifier, "value"},
        {TT::kPlus, "+"},
        {TT::kInt, "1"},
        {TT::kEnd, "end"},
        {TT::kFunction, "function"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, KeywordsRecognition) {
    std::string code;
    std::vector<ItmoScript::Token> expected;

    for (const auto& [keyword, type] : ItmoScript::kKeywords) {
        code += keyword;
        code += ' ';
        expected.push_back({.type = type, .literal = keyword});
    }

    expected.push_back({TT::kEOF, ""});
    ItmoScript::Lexer lexer{code};
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, BasicOperatorsUsage) {
    std::string code = R"(
        5 < 10
        5 > 10
        !true == false
        !false == true

        5 >= 3
        3 <= 40
    )";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kInt, "5"},
        {TT::kLess, "<"},
        {TT::kInt, "10"},
        {TT::kInt, "5"},
        {TT::kGreater, ">"},
        {TT::kInt, "10"},
        {TT::kBang, "!"},
        {TT::kTrue, "true"},
        {TT::kEqual, "=="},
        {TT::kFalse, "false"},
        {TT::kBang, "!"},
        {TT::kFalse, "false"},
        {TT::kEqual, "=="},
        {TT::kTrue, "true"},
        {TT::kInt, "5"},
        {TT::kGreaterOrEqual, ">="},
        {TT::kInt, "3"},
        {TT::kInt, "3"},
        {TT::kLessOrEqual, "<="},
        {TT::kInt, "40"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, IgnoringComments) {
    std::string code = R"(
        // very important function
        // very-very important function!!
        incr = function(value)
            // 239 239 end end 42
            return value + 1
        end function // 239 239
    )";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> expected = {
        {TT::kIdentifier, "incr"},
        {TT::kAssing, "="},
        {TT::kFunction, "function"},
        {TT::kLParen, "("},
        {TT::kIdentifier, "value"},
        {TT::kRParen, ")"},
        {TT::kReturn, "return"},
        {TT::kIdentifier, "value"},
        {TT::kPlus, "+"},
        {TT::kInt, "1"},
        {TT::kEnd, "end"},
        {TT::kFunction, "function"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}
