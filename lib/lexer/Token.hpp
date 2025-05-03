#pragma once

#include <string>
#include <map>
#include <set>

namespace ItmoScript {

enum class TokenType {
    kIllegal,
    kEOF,

    kIdentifier,

    // Literals
    kInt,
    kFloat,

    // Assignment operators
    kAssing,
    kPlusAssign,
    kMinusAssign,
    kAsteriskAssign,
    kSlashAssign,

    // Ariphmetic operators
    kPlus,
    kMinus,
    kAsterisk,
    kSlash,

    // Logical operators
    kLess,
    kGreater,
    kBang,
    kLessOrEqual,
    kGreaterOrEqual,
    kEqual,
    kNotEqual,

    // Punctuation
    kPoint,
    kComma,
    kLParen,
    kRParen,
    kLBracket,
    kRBracket,

    // Keywords
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

    // Keywords-literals
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

static const std::map<char, TokenType> kOneCharTokens{
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

static const std::set<char> kCompoundOpStarters = {
    '=', '+', '-', '*', '/', '<', '>', '!'
};

static const std::map<std::string, TokenType> kCompoundOperators{
    {"==", TokenType::kEqual},
    {"+=", TokenType::kPlusAssign},
    {"-=", TokenType::kMinusAssign},
    {"*=", TokenType::kAsteriskAssign},
    {"/=", TokenType::kSlashAssign},
    {"<=", TokenType::kLessOrEqual},
    {">=", TokenType::kGreaterOrEqual},
    {"!=", TokenType::kNotEqual},
};

static const std::map<std::string, TokenType> kKeywords{
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

static const std::map<TokenType, std::string> kTokenTypeNames = {
    {TokenType::kIllegal, "ILLEGAL"},
    {TokenType::kEOF, "EOF"},

    {TokenType::kIdentifier, "IDENTIFIER"},

    // Literals
    {TokenType::kInt, "INT"},
    {TokenType::kFloat, "FLOAT"},

    // Assignment operators
    {TokenType::kAssing, "="},
    {TokenType::kPlusAssign, "+="},
    {TokenType::kMinusAssign, "-="},
    {TokenType::kAsteriskAssign, "*="},
    {TokenType::kSlashAssign, "/="},

    // Ariphmetic operators
    {TokenType::kPlus, "+"},
    {TokenType::kMinus, "-"},
    {TokenType::kAsterisk, "*"},
    {TokenType::kSlash, "/"},

    // Logical operators
    {TokenType::kLess, "<"},
    {TokenType::kGreater, ">"},
    {TokenType::kBang, "!"},
    {TokenType::kLessOrEqual, "<="},
    {TokenType::kGreaterOrEqual, ">="},
    {TokenType::kEqual, "=="},
    {TokenType::kNotEqual, "!="},

    // Punctuation
    {TokenType::kPoint, "."},
    {TokenType::kComma, ","},
    {TokenType::kLParen, "("},
    {TokenType::kRParen, ")"},
    {TokenType::kLBracket, "["},
    {TokenType::kRBracket, "]"},

    // Keywords
    {TokenType::kFunction, "FUNCTION"},
    {TokenType::kEnd, "END"},
    {TokenType::kReturn, "RETURN"},
    {TokenType::kOr, "OR"},
    {TokenType::kAnd, "AND"},
    {TokenType::kNot, "NOT"},
    {TokenType::kIf, "IF"},
    {TokenType::kElse, "ELSE"},
    {TokenType::kFor, "FOR"},
    {TokenType::kWhile, "WHILE"},

    // Keywords-literals
    {TokenType::kTrue, "TRUE"},
    {TokenType::kFalse, "FALSE"},
    {TokenType::kNil, "NIL"}
};

} // namespace ItmoScript
