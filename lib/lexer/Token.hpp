#pragma once

#include <string>
#include <map>

namespace ItmoScript {

enum class TokenType {
    kIllegal,
    kEOF,

    kIdentifier,

    // literals
    kInt,
    kFloat,
    kNullLiteral,

    // operators
    kAssing,
    kPlus,
    kMinus,
    kMultiply,
    kSlash,

    kPoint,
    kComma,
    kLParen,
    kRParen,
    kLBracket,
    kRBracket,

    // keywords
    kFunction,
    kEnd,
    kReturn,
};

struct Token {
    TokenType type;
    std::string literal;

    bool operator==(const Token& other) const = default;
};

const std::map<char, TokenType> kSmallTokenMap{
    {EOF, TokenType::kEOF},
    {'=', TokenType::kAssing},
    {'+', TokenType::kPlus},
    {'-', TokenType::kMinus},
    {'*', TokenType::kMultiply},
    {'/', TokenType::kSlash},
    {',', TokenType::kComma},
    {'(', TokenType::kLParen},
    {')', TokenType::kRParen},
    {'[', TokenType::kLBracket},
    {']', TokenType::kRBracket},
    {'.', TokenType::kPoint},
};

const std::map<std::string, TokenType> kKeywords{
    {"function", TokenType::kFunction},
    {"end", TokenType::kEnd},
    {"nil", TokenType::kNullLiteral},
    {"return", TokenType::kReturn}
};

} // namespace ItmoScript
