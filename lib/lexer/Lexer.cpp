#include "Lexer.hpp"
#include "SyntaxError.hpp"

#include <algorithm>

namespace itmoscript {

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

    if (input_->eof()) {
        token.type = TokenType::kEOF;
    } else if (kOneCharTokens.contains(current)) {
        token = ReadCompoundToken();
    } else if (IsIdentifierBegin(current)) {
        std::string word = ReadWord();
        token = LookupIdentifier(word);
    } else if (std::isdigit(current)) {
        token = ReadNumber();
    } else if (current == '"') {
        token = ReadStringLiteral();
    } else if (current == '\n') {
        token.type = TokenType::kNewLine;
    } else {
        throw lang_exceptions::SyntaxError{current_line_, current_col_};
    }

    SetTokenPosition(token);
    last_token_ = token;

    current_col_ += token.literal.size();
    return token;
}

bool Lexer::HasNextToken() const {
    return !input_->eof();
}

char Lexer::ReadChar() {
    current_char_ = input_->get();
    return current_char_;
}

char Lexer::PeekChar() const {
    return input_->peek();
}

Token Lexer::ReadCompoundToken() {
    Token token;

    if (HasNextToken() && kCompoundOpStarters.contains(current_char_)) {
        char next_char = PeekChar();
        std::string op = std::string{current_char_, next_char};

        if (kCompoundOperators.contains(op)) {
            ReadChar();
            token.type = kCompoundOperators.at(op);
            token.literal = std::move(op);
        } else {
            token.type = kOneCharTokens.at(current_char_);
            token.literal = std::string{current_char_};
        }

        return token;
    }
    
    token.type = kOneCharTokens.at(current_char_);
    token.literal = std::string{current_char_};

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
    std::string word(1, current_char_);
    while (IsIdentifierChar(PeekChar())) {
        word += ReadChar();
    }
    return word;
}

Token Lexer::ReadNumber() {
    std::string word(1, current_char_);
    while (std::isdigit(PeekChar())) {
        word += ReadChar();
    }

    Token token;
    token.type = TokenType::kInt;

    if (PeekChar() == '.') {
        word += ReadChar();
        while (std::isdigit(PeekChar())) {
            word += ReadChar();
        }
        token.type = TokenType::kFloat;
    }

    // TODO: add notation (1.23e-1)

    if (IsIdentifierChar(PeekChar())) {
        throw lang_exceptions::SyntaxError{current_line_, current_col_ + word.size()};
    }

    SetTokenPosition(token);
    token.literal = std::move(word);
    return token;
}

Token Lexer::ReadStringLiteral() {
    Token token;
    token.type = TokenType::kStringLiteral;
    SetTokenPosition(token);

    std::string word(1, current_char_);
    while (PeekChar() != '"') {
        if (!HasNextToken()) {
            throw lang_exceptions::SyntaxError{
                current_line_, 
                current_col_ + word.size(), 
                "unexpected end of a string"
            };
        }
        
        word += ReadChar();
        if (current_char_ == '\\') {
            word += ReadChar();
        }
    }

    word += ReadChar();
    token.literal = std::move(word);
    return token;
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