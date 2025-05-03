#pragma once

#include <string>
#include <map>
#include <set>

namespace ItmoScript {

enum class TokenType {
    kIllegal,
    kEOF,

    kIdentifier,

    // literals
    kInt,
    kFloat,

    // assignment operators
    kAssing,
    kPlusAssign,
    kMinusAssign,
    kAsteriskAssign,
    kSlashAssign,

    // ariphmetic operators
    kPlus,
    kMinus,
    kAsterisk,
    kSlash,

    // logical operators
    kLess,
    kGreater,
    kBang,
    kLessOrEqual,
    kGreaterOrEqual,
    kEqual,
    kNotEqual,

    // punctuation
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
    kOr,
    kAnd,
    kNot,
    kIf,
    kElse,
    kFor,
    kWhile,

    // keywords-literals
    kTrue,
    kFalse,
    kNil,
};

struct Token {
    TokenType type;
    std::string literal;
    size_t line;
    size_t column;

    bool operator==(const Token& other) const {
        return type == other.type && literal == other.literal;
    }
};

const std::map<char, TokenType> kOneCharTokens{
    {'=', TokenType::kAssing},
    {'+', TokenType::kPlus},
    {'-', TokenType::kMinus},
    {'*', TokenType::kAsterisk},
    {'/', TokenType::kSlash},
    {'<', TokenType::kLess},
    {'>', TokenType::kGreater},
    {'!', TokenType::kBang},
    {'.', TokenType::kPoint},
    {',', TokenType::kComma},
    {'(', TokenType::kLParen},
    {')', TokenType::kRParen},
    {'[', TokenType::kLBracket},
    {']', TokenType::kRBracket},
    {EOF, TokenType::kEOF},
};

const std::set<char> kCompoundOpStarters = {
    '=', '+', '-', '*', '/', '<', '>', '!'
};

const std::map<std::string, TokenType> kCompoundOperators{
    {"==", TokenType::kEqual},
    {"+=", TokenType::kPlusAssign},
    {"-=", TokenType::kMinusAssign},
    {"*=", TokenType::kAsteriskAssign},
    {"/=", TokenType::kSlashAssign},
    {"<=", TokenType::kLessOrEqual},
    {">=", TokenType::kGreaterOrEqual},
    {"!=", TokenType::kNotEqual},
};

const std::map<std::string, TokenType> kKeywords{
    {"function", TokenType::kFunction},
    {"end", TokenType::kEnd},
    {"nil", TokenType::kNil},
    {"return", TokenType::kReturn},
    {"or", TokenType::kOr},
    {"and", TokenType::kAnd},
    {"not", TokenType::kNot},
    {"true", TokenType::kTrue},
    {"false", TokenType::kFalse},
    {"if", TokenType::kIf},
    {"else", TokenType::kElse},
    {"for", TokenType::kFor},
    {"while", TokenType::kWhile},
};

} // namespace ItmoScript
