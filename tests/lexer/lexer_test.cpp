#include "lexer_test.hpp"

TEST(LexerTestSuite, OneCharTokenTest) {
    std::string code;
    std::vector<itmoscript::Token> expected;

    for (const auto& [token, type] : itmoscript::kOneCharTokens) {
        expected.push_back({.type = type, .literal = std::string{token}});
        code += token;
        code += ' ';
    }

    expected.push_back({.type = TT::kEOF});
    itmoscript::Lexer lexer{code};
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, OneCharIdentiriersTest) {
    std::string code = "a b c d e f";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
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

TEST(LexerTestSuite, IdentifiersAssignmentTest) {
    std::string code = "n = number_2";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kIdentifier, "n"},
        {TT::kAssign, "="},
        {TT::kIdentifier, "number_2"},
    };

    expected.push_back({.type = TT::kEOF});
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, FunctionCreationTest) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function
    )";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kNewLine, "\n"},
        {TT::kIdentifier, "incr"},
        {TT::kAssign, "="},
        {TT::kFunction, "function"},
        {TT::kLParen, "("},
        {TT::kIdentifier, "value"},
        {TT::kRParen, ")"},
        {TT::kNewLine, "\n"},
        {TT::kReturn, "return"},
        {TT::kIdentifier, "value"},
        {TT::kPlus, "+"},
        {TT::kInt, "1"},
        {TT::kNewLine, "\n"},
        {TT::kEnd, "end"},
        {TT::kFunction, "function"},
        {TT::kNewLine, "\n"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, KeywordsRecognitionTest) {
    std::string code;
    std::vector<itmoscript::Token> expected;

    for (const auto& [keyword, type] : itmoscript::kKeywords) {
        code += keyword;
        code += ' ';
        expected.push_back({.type = type, .literal = keyword});
    }

    expected.push_back({TT::kEOF, ""});
    itmoscript::Lexer lexer{code};
    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, BasicOperatorsUsageTest) {
    std::string code = R"(
        5 < 10
        5 > 10
        !true == false
        !false == true

        5 >= 3
        3 <= 40
    )";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kNewLine, "\n"},
        {TT::kInt, "5"},
        {TT::kLess, "<"},
        {TT::kInt, "10"},
        {TT::kNewLine, "\n"},
        {TT::kInt, "5"},
        {TT::kGreater, ">"},
        {TT::kInt, "10"},
        {TT::kNewLine, "\n"},
        {TT::kBang, "!"},
        {TT::kTrue, "true"},
        {TT::kEqual, "=="},
        {TT::kFalse, "false"},
        {TT::kNewLine, "\n"},
        {TT::kBang, "!"},
        {TT::kFalse, "false"},
        {TT::kEqual, "=="},
        {TT::kTrue, "true"},
        {TT::kNewLine, "\n"},
        {TT::kNewLine, "\n"},
        {TT::kInt, "5"},
        {TT::kGreaterOrEqual, ">="},
        {TT::kInt, "3"},
        {TT::kNewLine, "\n"},
        {TT::kInt, "3"},
        {TT::kLessOrEqual, "<="},
        {TT::kInt, "40"},
        {TT::kNewLine, "\n"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, IgnoringCommentsTest) {
    std::string code = R"(
        // very important function
        // very-very important function!!
        incr = function(value)
            // 239 239 end end 42
            return value + 1
        end function // 239 239
    )";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kNewLine, "\n"},
        {TT::kIdentifier, "incr"},
        {TT::kAssign, "="},
        {TT::kFunction, "function"},
        {TT::kLParen, "("},
        {TT::kIdentifier, "value"},
        {TT::kRParen, ")"},
        {TT::kNewLine, "\n"},
        {TT::kReturn, "return"},
        {TT::kIdentifier, "value"},
        {TT::kPlus, "+"},
        {TT::kInt, "1"},
        {TT::kNewLine, "\n"},
        {TT::kEnd, "end"},
        {TT::kFunction, "function"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, WhileLoopTest) {
    std::string code = R"(
        while i < 10
            i += 1
        end while
    )";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kNewLine, "\n"},
        {TT::kWhile, "while"},
        {TT::kIdentifier, "i"},
        {TT::kLess, "<"},
        {TT::kInt, "10"},
        {TT::kNewLine, "\n"},
        {TT::kIdentifier, "i"},
        {TT::kPlusAssign, "+="},
        {TT::kInt, "1"},
        {TT::kNewLine, "\n"},
        {TT::kEnd, "end"},
        {TT::kWhile, "while"},
        {TT::kNewLine, "\n"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}

TEST(LexerTestSuite, ForLoopTest) {
    std::string code = R"(
        for i in arr
            i += 1
        end for
    )";

    itmoscript::Lexer lexer{code};

    std::vector<itmoscript::Token> expected = {
        {TT::kNewLine, "\n"},
        {TT::kFor, "for"},
        {TT::kIdentifier, "i"},
        {TT::kIn, "in"},
        {TT::kIdentifier, "arr"},
        {TT::kNewLine, "\n"},
        {TT::kIdentifier, "i"},
        {TT::kPlusAssign, "+="},
        {TT::kInt, "1"},
        {TT::kNewLine, "\n"},
        {TT::kEnd, "end"},
        {TT::kFor, "for"},
        {TT::kNewLine, "\n"},
        {TT::kEOF, ""}
    };

    CompareTokens(lexer, expected);
}
