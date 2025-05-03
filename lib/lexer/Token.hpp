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
    {TokenType::kAssing, "ASSIGN"},
    {TokenType::kPlusAssign, "PLUS_ASSIGN"},
    {TokenType::kMinusAssign, "MINUS_ASSIGN"},
    {TokenType::kAsteriskAssign, "ASTERISK_ASSIGN"},
    {TokenType::kSlashAssign, "SLASH_ASSIGN"},

    // Ariphmetic operators
    {TokenType::kPlus, "PLUS"},
    {TokenType::kMinus, "MINUS"},
    {TokenType::kAsterisk, "ASTERISK"},
    {TokenType::kSlash, "SLASH"},

    // Logical operators
    {TokenType::kLess, "LESS"},
    {TokenType::kGreater, "GREATER"},
    {TokenType::kBang, "BANG"},
    {TokenType::kLessOrEqual, "LESS_OR_EQUAL"},
    {TokenType::kGreaterOrEqual, "GREATER_OR_EQUAL"},
    {TokenType::kEqual, "EQUAL"},
    {TokenType::kNotEqual, "NOT_EQUAL"},

    // Punctuation
    {TokenType::kPoint, "POINT"},
    {TokenType::kComma, "COMMA"},
    {TokenType::kLParen, "L_PAREN"},
    {TokenType::kRParen, "R_PAREN"},
    {TokenType::kLBracket, "L_BRACKET"},
    {TokenType::kRBracket, "R_BRACKET"},

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
