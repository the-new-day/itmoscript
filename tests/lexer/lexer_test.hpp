#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <format>

#include "lib/lexer/Lexer.hpp"

using TT = itmoscript::TokenType;

// static std::string Join(const std::vector<itmoscript::Token>& tokens, const std::string& glue = "") {
//     std::string result;
//     for (size_t i = 0; i < tokens.size(); ++i) {
//         result += tokens[i].literal;
//         if (i != tokens.size() - 1) result += glue;
//     }
//     return result;
// }

static void CompareTokens(itmoscript::Lexer& lexer, const std::vector<itmoscript::Token>& expected) {
    std::vector<itmoscript::Token> tokens;

    while (lexer.HasNextToken()) {
        itmoscript::Token token = lexer.GetNextToken();
        tokens.push_back(std::move(token));
    }

    if (!tokens.empty() && tokens.back().type != TT::kEOF) {
        tokens.push_back({.type = TT::kEOF});
    }
    
    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        itmoscript::Token real_token = tokens[i];
        itmoscript::Token expected_token = expected[i];

        ASSERT_EQ(real_token.type, expected_token.type) 
            << std::format(
                "expected {}, got {} for token {}\n", 
                itmoscript::kTokenTypeNames.at(expected_token.type),
                itmoscript::kTokenTypeNames.at(real_token.type),
                i
            );
            
        ASSERT_EQ(real_token.literal, expected_token.literal)
            << std::format(
                "expected {}, got {} for token {}\n", 
                real_token.literal,
                expected_token.literal,
                i
            );
    }
}
