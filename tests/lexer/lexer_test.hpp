#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "lib/lexer/Lexer.hpp"

using TT = ItmoScript::TokenType;

static void CompareTokens(ItmoScript::Lexer& lexer, const std::vector<ItmoScript::Token>& expected) {
    std::vector<ItmoScript::Token> tokens;

    while (lexer.HasNextToken()) {
        tokens.push_back(lexer.GetNextToken());
    }

    if (!tokens.empty() && tokens.back().type != TT::kEOF) {
        tokens.push_back({.type = TT::kEOF});
    }
    
    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        ItmoScript::Token real_token = tokens[i];
        ItmoScript::Token expected_token = expected[i];

        ASSERT_EQ(real_token.type, expected_token.type);
        ASSERT_EQ(real_token.literal, expected_token.literal);
    }
}
