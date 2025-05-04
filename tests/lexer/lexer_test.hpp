#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <format>

#include "lib/lexer/Lexer.hpp"

using TT = ItmoScript::TokenType;

static void CompareTokens(ItmoScript::Lexer& lexer, const std::vector<ItmoScript::Token>& expected) {
    std::vector<ItmoScript::Token> tokens;

    while (lexer.HasNextToken()) {
        ItmoScript::Token token = lexer.GetNextToken();
        tokens.push_back(std::move(token));
    }

    if (!tokens.empty() && tokens.back().type != TT::kEOF) {
        tokens.push_back({.type = TT::kEOF});
    }
    
    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        ItmoScript::Token real_token = tokens[i];
        ItmoScript::Token expected_token = expected[i];

        ASSERT_EQ(real_token.type, expected_token.type) 
            << std::format(
                "expected {}, got {} for token {}\n", 
                ItmoScript::kTokenTypeNames.at(expected_token.type),
                ItmoScript::kTokenTypeNames.at(real_token.type),
                i
            );
            
        ASSERT_EQ(real_token.literal, expected_token.literal);
    }
}
