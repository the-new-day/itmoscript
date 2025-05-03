#include <lib/lexer/Lexer.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>

TEST(LexerTestSuite, OneCharToken) {
    std::string code = "=+()[],";

    ItmoScript::Lexer lexer{code};

    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kAssing);
    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kPlus);
    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kLParen);
    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kRParen);
    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kLBracket);
    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kRBracket);
    ASSERT_EQ(lexer.GetNextToken().type, ItmoScript::TokenType::kComma);
}

TEST(LexerTestSuite, ReadIntAndFloat) {
    std::string code = "123 9.1 33.091234 900";

    ItmoScript::Lexer lexer{code};
    using TT = ItmoScript::TokenType;

    std::vector<ItmoScript::Token> expected = {
        {TT::kInt, "123"},
        {TT::kFloat, "9.1"},
        {TT::kFloat, "33.091234"},
        {TT::kInt, "900"}
    };

    std::vector<ItmoScript::Token> tokens;

    while (lexer.HasNextToken()) {
        tokens.push_back(lexer.GetNextToken());
    }
    
    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        ItmoScript::Token real_token = tokens[i];
        ItmoScript::Token expected_token = expected[i];

        ASSERT_EQ(real_token.type, expected_token.type);
        ASSERT_EQ(real_token.literal, expected_token.literal);
    }
}

TEST(LexerTestSuite, IdentifiersAssignment) {
    std::string code = "number_1 = number_2";

    ItmoScript::Lexer lexer{code};

    ItmoScript::Token token1 = lexer.GetNextToken();
    ItmoScript::Token token2 = lexer.GetNextToken();
    ItmoScript::Token token3 = lexer.GetNextToken();

    ASSERT_EQ(token1.type, ItmoScript::TokenType::kIdentifier);
    ASSERT_EQ(token1.literal, "number_1");

    ASSERT_EQ(token2.type, ItmoScript::TokenType::kAssing);
    ASSERT_EQ(token2.literal, "=");

    ASSERT_EQ(token3.type, ItmoScript::TokenType::kIdentifier);
    ASSERT_EQ(token3.literal, "number_2");
}

TEST(LexerTestSuite, FunctionCreation) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function
    )";

    using TT = ItmoScript::TokenType;

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

    std::vector<ItmoScript::Token> tokens;

    while (lexer.HasNextToken()) {
        tokens.push_back(lexer.GetNextToken());
    }
    
    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        ItmoScript::Token real_token = tokens[i];
        ItmoScript::Token expected_token = expected[i];

        ASSERT_EQ(real_token.type, expected_token.type);
        ASSERT_EQ(real_token.literal, expected_token.literal);
    }
}
