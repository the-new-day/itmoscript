#include "Lexer.hpp"

#include <algorithm>

namespace ItmoScript {

Token Lexer::GetNextToken() {
    char current = ReadChar();

    while (std::isspace(current)) {
        if (current == '\n') {
            ++current_line_;
            current_col_ = 0;
            break;
        }

        ++current_col_;
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
    } else if (current == '\n') {
        token.type = TokenType::kNewLine;
    } else {
        token = Token{
            .type = TokenType::kIllegal,
            .literal = std::string{current}
        };
    }

    SetTokenPosition(token);
    last_token_ = token;

    current_col_ += token.literal.size();
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
    Token token;

    if (HasNextToken() && kCompoundOpStarters.contains(current)) {
        char next_char = code_[read_pos_];
        std::string op = std::string{current, next_char};

        if (kCompoundOperators.contains(op)) {
            ++read_pos_;
            token.type = kCompoundOperators.at(op);
            token.literal = std::move(op);
        } else {
            token.type = kOneCharTokens.at(current);
            token.literal = std::string{current};
        }

        return token;
    }
    
    token.type = kOneCharTokens.at(current);
    token.literal = std::string{current};

    SetTokenPosition(token);
    return token;
}

void Lexer::SkipComment() {
    while (PeekChar() != '\n' && PeekChar() != EOF) {
        ReadChar();
    }

    if (PeekChar() == '\n') ReadChar();
    ++current_line_;
    current_col_ = 0;
}

void Lexer::SetTokenPosition(Token& token) const {
    token.line = current_line_;
    token.column = current_col_;
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

    return {.type = type, .literal = word, .line = current_line_, .column = current_col_};
}

Token Lexer::LookupIdentifier(const std::string& word) const {
    Token token;
    SetTokenPosition(token);

    if (kKeywords.contains(word)) {
        token.type = kKeywords.at(word);
    } else {
        token.type = TokenType::kIdentifier;
    }

    token.literal = word;
    return token;
}

bool Lexer::IsIdentifierChar(char ch) {
    return std::isalnum(ch) || ch == '_';
}

bool Lexer::IsIdentifierBegin(char ch) {
    return std::isalpha(ch) || ch == '_';
}

} // namespace ItmoScript
