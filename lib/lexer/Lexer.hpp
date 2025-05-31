#pragma once

#include "Token.hpp"

#include <string>
#include <utility>
#include <cstddef>
#include <istream>
#include <memory>
#include <sstream>

namespace itmoscript {

/**
 * @class Lexer
 * @brief Reads the sequence of characters of the source code and turns them into tokens
 * for later building of the AST and evaluation.
 */
class Lexer {
public:
    explicit Lexer(const std::string& code)
        : owned_input_(std::make_unique<std::istringstream>(code))
        , input_(owned_input_.get()) {}

    /**
     * @brief Constructs a lexer from an existing input stream.
     * The lexer does not take ownership of the stream, and the caller must ensure it remains valid.
     */
    explicit Lexer(std::istream& input)
        : input_(&input) {}

    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer& operator=(Lexer&&) = delete;
    ~Lexer() = default;

    /** 
     * @brief Reads the next token from the stream.
     * If encounters an invalid token, processes it and returns as a TokenType::kIllegal token.
     */
    Token GetNextToken();

    /**
     * @brief Checks whether next token can be retrieved or not.
     * Specifically, checks if the input stream is already at EOF.
     */
    bool HasNextToken() const;

    /** 
     * @brief Checks if ch can be a part of an identifier, but not the beginning.
     * Specifically, checks is ch is an alphanumeric character or an underscore.
     */
    static bool IsIdentifierChar(char ch);

    /** 
     * @brief Checks if ch can be the beginning of an identifier.
     * Specifically, checks is ch is a letter or an underscore.
     */
    static bool IsIdentifierBegin(char ch);

private:
    std::unique_ptr<std::istream> owned_input_;
    std::istream* input_;

    Token last_token_{.type = TokenType::kEOF};
    char current_char_ = 0;
    char prev_char_ = 0;

    size_t current_line_ = 1;
    size_t current_col_ = 1;

    /** 
     * @brief Reads the next character from the input and updates current_char_.
     * Uupdates current_line_ and current_col_.
     */
    char ReadChar();

    /** @brief Peeks the next character from the input without consuming it. */
    char PeekChar() const;

    /**
     * @brief Reads current word by continuesly adding the characters while they fulfill
     * the requirement of an identifier char.
     * @see IsIdentifierChar For info about identifier chars.
     */
    std::string ReadWord();

    /**
     * @brief Reads an integer or a float number starting from the current character.
     * Assumes that current_char_ is already a digit.
     * @todo Add support for notation, binary representation, octal, hex
     */
    Token ReadNumber();

    /**
     * @brief Reads a string literal enclosed in double quotes.
     * Supports escape sequences (e.g. \" \\n), but doesn't parse them (leaves as a 2-character sequence).
     */
    Token ReadStringLiteral();

    /**
     * @brief Tries to read the compound token (e.g. a 2-character operator). If succeeds, returns it as a valid token.
     * Otherwise, returns the last read characted as a token.
     * Must be called only if the current character is a valid one-char token.
     */
    Token ReadCompoundToken();

    /**
     * @brief Skips all the characters before meets the new line or the EOF.
     */
    void SkipComment();

    /**
     * @brief Sets the token's line and column position to the beginning of the token.
     * Should be called right after token creation.
     */
    void SetTokenPosition(Token& token) const;

    /**
     * @brief Reads the word and turns it either to a corresponding keyword-token or to an identifier-token,
     * depending on whether the given word is a keyword.
     */
    Token LookupIdentifier(const std::string& word) const;
};
    
} // namespace itmoscript
