#include "Lexer.hpp"

#include <algorithm>

namespace ItmoScript {

Token Lexer::GetNextToken() {
    char last_char = ' ';

    while (std::isspace(last_char)) {
        last_char = ReadChar();
    }

    Token token;

    if (last_char == EOF) {
        token = {.type = TokenType::kEOF};
    } else if (kOneCharTokens.contains(last_char)) {
        token = ReadOperator();
    } else if (std::isalpha(last_char)) {
        std::string word = ReadWord();
        token = LookupIdentifier(word);
    } else if (std::isdigit(last_char)) {
        token = Token{.type = TokenType::kInt, .literal = ReadNumber()};
    } else {
        token = Token{.type = TokenType::kIllegal};
    }

    last_token_ = token;
    return token;
}

bool Lexer::HasNextToken() const {
    return last_token_.type != TokenType::kEOF && read_pos_ < code_.size();
}

char Lexer::ReadChar() {
    char ch = PeekChar();
    ++read_pos_;
    return ch;
}

char Lexer::PeekChar() const {
    if (!HasNextToken()) {
        return EOF;
    }

    return code_[read_pos_];
}

Token Lexer::ReadOperator() {
    char last_char = code_[read_pos_ - 1];

    if (HasNextToken() && kCompoundOpStarters.contains(last_char)) {
        char next_char = code_[read_pos_];
        std::string op = std::string{last_char, next_char};

        if (kCompoundOperators.contains(op)) {
            ++read_pos_;
            return Token{.type = kCompoundOperators.at(op), .literal = op};
        }

        return Token{.type = kOneCharTokens.at(last_char), .literal = std::string{last_char}};
    }

    return Token{.type = kOneCharTokens.at(last_char), .literal = std::string{last_char}};
}

std::string Lexer::ReadWord() {
    if (!HasNextToken()) {
        return std::string{};
    }

    auto next_nonalnum = std::find_if(
        code_.begin() + read_pos_, 
        code_.end(), 
        [](char ch) { return !IsIdentifierChar(ch); }
    );

    std::string word = std::string(code_.begin() + read_pos_ - 1, next_nonalnum);
    read_pos_ += word.size() - 1;
    return word;
}

std::string Lexer::ReadNumber() {
    if (!HasNextToken()) {
        return std::string{};
    }

    auto next_nondigit = std::find_if(
        code_.begin() + read_pos_, 
        code_.end(), 
        [](char ch) { return !std::isdigit(ch); }
    );

    std::string word = std::string(code_.begin() + read_pos_ - 1, next_nondigit);
    read_pos_ += word.size() - 1;

    return word;
}

Token Lexer::LookupIdentifier(const std::string& word) {
    if (kKeywords.contains(word)) {
        return Token{.type = kKeywords.at(word), .literal = word};
    }

    return Token{.type = TokenType::kIdentifier, .literal = word};
}

bool Lexer::IsIdentifierChar(char ch) {
    return std::isalnum(ch) || ch == '_';
}

} // namespace ItmoScript
