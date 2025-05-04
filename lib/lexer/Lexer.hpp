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

private:
    std::string code_;
    size_t read_pos_{0};
    Token last_token_{.type = TokenType::kIllegal};

    size_t current_line_{0};
    size_t current_col_{0};

    char ReadChar();
    char PeekChar() const;
    std::string ReadWord();
    Token ReadNumber();

    // If last read character was a compound operator starter, tries to read compound operator and return it as a token.
    // If fails, returns the last read character as a token.
    Token ReadCompoundToken();

    void SkipComment();

    // Fills token's line and column fields
    void SetTokenPosition(Token& token) const;

    // If word is a keyword, returns it with a corresponding TokenType.
    // Otherwise, returns word with TokeyType::kIdentifier type
    Token LookupIdentifier(const std::string& word) const;
};
    
} // namespace ItmoScript
