#include "Lexer.hpp"

#include <algorithm>

namespace ItmoScript {

Token Lexer::GetNextToken() {
    char current = ReadChar();

    while (std::isspace(current)) {
        current = ReadChar();
    }

    if (current == '/' && PeekChar() == '/') {
        SkipComment();
        return GetNextToken();
    }

    Token token;

    if (current == EOF) {
        token = {.type = TokenType::kEOF};
    } else if (kOneCharTokens.contains(current)) {
        token = ReadCompoundToken();
    } else if (IsIdentifierBegin(current)) {
        std::string word = ReadWord();
        token = LookupIdentifier(word);
    } else if (std::isdigit(current)) {
        token = ReadNumber();
    } else {
        token = Token{.type = TokenType::kIllegal, .literal = std::string{current}};
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

Token Lexer::ReadCompoundToken() {
    char current = code_[read_pos_ - 1];

    if (HasNextToken() && kCompoundOpStarters.contains(current)) {
        char next_char = code_[read_pos_];
        std::string op = std::string{current, next_char};

        if (kCompoundOperators.contains(op)) {
            ++read_pos_;
            return Token{.type = kCompoundOperators.at(op), .literal = op};
        }

        return Token{.type = kOneCharTokens.at(current), .literal = std::string{current}};
    }

    return Token{.type = kOneCharTokens.at(current), .literal = std::string{current}};
}

void Lexer::SkipComment() {
    while (PeekChar() != '\n' && PeekChar() != EOF) {
        ReadChar();
    }

    if (PeekChar() == '\n') ReadChar();
}

std::string Lexer::ReadWord() {
    auto next_nonalnum = std::find_if(
        code_.begin() + read_pos_, 
        code_.end(), 
        [](char ch) { return !IsIdentifierChar(ch); }
    );

    std::string word = std::string(code_.begin() + read_pos_ - 1, next_nonalnum);
    read_pos_ += word.size() - 1;
    return word;
}

Token Lexer::ReadNumber() {
    auto not_digit = [](char ch) { return !std::isdigit(ch); };

    auto next_token_char = std::find_if(
        code_.begin() + read_pos_, 
        code_.end(), 
        not_digit
    );

    std::string word = std::string(code_.begin() + read_pos_ - 1, next_token_char);
    read_pos_ += word.size() - 1;

    TokenType type = TokenType::kInt;

    if (next_token_char != code_.end() && *next_token_char == '.') {
        next_token_char = std::find_if(
            code_.begin() + read_pos_ + 1, 
            code_.end(), 
            not_digit
        );

        word += std::string(code_.begin() + read_pos_, next_token_char);
        read_pos_ += word.size() - 1;
        type = TokenType::kFloat;
    }

    return {.type = type, .literal = word};
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

bool Lexer::IsIdentifierBegin(char ch) {
    return std::isalpha(ch) || ch == '_';
}

} // namespace ItmoScript
