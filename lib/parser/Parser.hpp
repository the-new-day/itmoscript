#pragma once

#include "lexer/Lexer.hpp"
#include "ast/AST.hpp"

#include <optional>
#include <functional>
#include <map>

namespace ItmoScript {

enum class Precedence {
    kLowest = 0,
    kEquals = 1,      // ==
    kLessGreater = 2, // < or >
    kSum = 3,         // +
    kProduct = 4,     // *
    kPrefix = 5,      // -x or !x
    kCall = 6,        // func(x)
};

const std::map<TokenType, Precedence> kPrecedences{
    {TokenType::kEqual, Precedence::kEquals},
    {TokenType::kNotEqual, Precedence::kEquals},
    {TokenType::kLess, Precedence::kLessGreater},
    {TokenType::kGreater, Precedence::kLessGreater},
    {TokenType::kPlus, Precedence::kSum},
    {TokenType::kMinus, Precedence::kSum},
    {TokenType::kAsterisk, Precedence::kProduct},
    {TokenType::kSlash, Precedence::kProduct},
};

class Parser {
public:
    Parser(Lexer& lexer);

    Parser(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser& operator=(Parser&&) = delete;
    ~Parser() = default;

    void ReadNextToken();
    Program ParseProgram();

    const std::vector<std::string>& GetErrors() const;

private:
    Lexer& lexer_;
    Token current_token_;
    Token peek_token_;

    std::vector<std::string> errors_;

    std::unique_ptr<Statement> ParseStatement();
    std::unique_ptr<AssignStatement> ParseAssignStatement();
    std::unique_ptr<ReturnStatement> ParseReturnStatement();
    std::unique_ptr<ExpressionStatement> ParseExpressionStatement();
    std::unique_ptr<Expression> ParseExpression(Precedence precedence);

    bool IsCurrentToken(TokenType type) const;
    bool IsPeekToken(TokenType type) const;
    bool ExpectPeek(TokenType type);

    void AddError(const std::string& msg);
    void AddError(std::string&& msg);
    void AddUnknownTokenError();
    void PeekError(TokenType expected_type);
    void AddNoPrefixFuncError(TokenType type);

    bool IsEndOfExpression(TokenType type);

    Precedence PeekPrecedence() const;
    Precedence GetCurrentPrecedence() const;

    using PrefixParseFunc = std::function<std::unique_ptr<Expression>(void)>;
    using InfixParseFunc = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

    std::map<TokenType, PrefixParseFunc> prefix_parse_funcs_;
    std::map<TokenType, InfixParseFunc> infix_parse_funcs_;

    std::unique_ptr<Identifier> ParseIdentifier();
    std::unique_ptr<IntegerLiteral> ParseIntegerLiteral();
    std::unique_ptr<PrefixExpression> ParsePrefixExpression();
    std::unique_ptr<InfixExpression> ParseInfixExpression(std::unique_ptr<Expression> left);
};
    
} // namespace ItmoScript
