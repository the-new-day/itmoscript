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

    // Checks if ch can be a beginning of an identifier
    static bool IsIdentifierBegin(char ch);

    // If word is a keyword, returns it with a corresponding TokenType.
    // Otherwise, returns word with TokeyType::kIdentifier type
    static Token LookupIdentifier(const std::string& word);

private:
    std::string code_;
    size_t read_pos_{0};
    Token last_token_{.type = TokenType::kIllegal};

    char ReadChar();
    char PeekChar() const;
    std::string ReadWord();
    std::string ReadNumber();

    // If last read character was a compound operator starter, tries to read compound operator and return it as a token.
    // If fails, returns the last read character as a token.
    Token ReadCompoundToken();

    void SkipComments();
};
    
} // namespace ItmoScript
