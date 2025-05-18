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
    kStringLiteral,

    // Assignment operators
    kAssign,
    kPlusAssign,
    kMinusAssign,
    kAsteriskAssign,
    kSlashAssign,
    kPercentAssign,
    kPowAssign,

    // Ariphmetic operators
    kPlus,
    kMinus,
    kAsterisk,
    kSlash,
    kPercent,
    kPow,

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
    kColon,
    kNewLine,

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
    kThen,
    kBreak,
    kContinue,
    kIn,

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
    {'=', TokenType::kAssign},
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
    {':', TokenType::kColon},
    {'%', TokenType::kPercent},
    {'^', TokenType::kPow},
    {'\n', TokenType::kNewLine},
    {EOF, TokenType::kEOF},
};

static const std::set<char> kCompoundOpStarters = {
    '=', '+', '-', '*', '/', '%', '^', '<', '>', '!',
};

static const std::map<std::string, TokenType> kCompoundOperators{
    {"==", TokenType::kEqual},
    {"+=", TokenType::kPlusAssign},
    {"-=", TokenType::kMinusAssign},
    {"*=", TokenType::kAsteriskAssign},
    {"/=", TokenType::kSlashAssign},
    {"%=", TokenType::kPercentAssign},
    {"^=", TokenType::kPowAssign},
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
    {"then", TokenType::kThen},
    {"break", TokenType::kBreak},
    {"continue", TokenType::kContinue},
    {"in", TokenType::kIn},
};

static const std::set<TokenType> kBlockTypes {
    TokenType::kIf,
    TokenType::kFor,
    TokenType::kWhile,
    TokenType::kFunction,
    TokenType::kElse,
};

static const std::map<TokenType, std::string> kTokenTypeNames = {
    {TokenType::kIllegal, "ILLEGAL"},
    {TokenType::kEOF, "EOF"},

    {TokenType::kIdentifier, "IDENTIFIER"},

    // Literals
    {TokenType::kInt, "INT"},
    {TokenType::kFloat, "FLOAT"},
    {TokenType::kStringLiteral, "STRING_LITERAL"},

    // Assignment operators
    {TokenType::kAssign, "="},
    {TokenType::kPlusAssign, "+="},
    {TokenType::kMinusAssign, "-="},
    {TokenType::kAsteriskAssign, "*="},
    {TokenType::kSlashAssign, "/="},
    {TokenType::kPercentAssign, "%="},
    {TokenType::kPowAssign, "^="},

    // Ariphmetic operators
    {TokenType::kPlus, "+"},
    {TokenType::kMinus, "-"},
    {TokenType::kAsterisk, "*"},
    {TokenType::kSlash, "/"},
    {TokenType::kPercent, "%"},
    {TokenType::kPow, "^"},

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
    {TokenType::kColon, ":"},
    {TokenType::kNewLine, "NEW_LINE"},

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
    {TokenType::kThen, "THEN"},
    {TokenType::kBreak, "BREAK"},
    {TokenType::kContinue, "CONTINUE"},
    {TokenType::kIn, "IN"},

    // Keywords-literals
    {TokenType::kTrue, "TRUE"},
    {TokenType::kFalse, "FALSE"},
    {TokenType::kNil, "NIL"},
};

} // namespace ItmoScript
