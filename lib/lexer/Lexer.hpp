#pragma once

#include "Token.hpp"

#include <string>
#include <utility>
#include <cstddef>

namespace ItmoScript {

class Lexer {
public:
    Lexer(std::string code)
        : code_(std::move(code)) {}

    Token GetNextToken();
    bool HasNextToken() const;

    // Checks if ch can be a part of an identifier (not the beginning!)
    static bool IsIdentifierChar(char ch);

    // If word is a keyword, returns it with a corresponding TokenType.
    // Otherwise, returns word with TokeyType::kIdentifier type
    static Token LookupIdentifier(const std::string& word);

private:
    std::string code_;
    size_t read_pos_{0};
    Token last_token_{.type = TokenType::kIllegal};

    char ReadChar();
    std::string ReadWord();
    std::string ReadNumber();
};
    
} // namespace ItmoScript
