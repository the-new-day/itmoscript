#pragma once

#include "lexer/Lexer.hpp"
#include "ast/AST.hpp"

#include <optional>
#include <functional>
#include <map>

namespace ItmoScript {

/**
 * @enum Precedence
 * @brief Determines the operator priority in expressions, from lowest to highest.
 * Operators with higher precedence bind tighter.
 */
enum class Precedence {
    kLowest = 1,
    kEquals = 2,      // ==
    kLessGreater = 3, // < or >
    kSum = 4,         // +
    kProduct = 5,     // *
    kPrefix = 6,      // -x or !x or x ^ y
    kCall = 7,        // func(x)
};

/**
 * @brief Mapping of token types to their corresponding precedence levels.
 * Used to resolve ambiguity in expression parsing (e.g., + vs * vs ^).
 */
const std::map<TokenType, Precedence> kPrecedences{
    {TokenType::kEqual, Precedence::kEquals},
    {TokenType::kNotEqual, Precedence::kEquals},
    {TokenType::kLess, Precedence::kLessGreater},
    {TokenType::kGreater, Precedence::kLessGreater},
    {TokenType::kLessOrEqual, Precedence::kLessGreater},
    {TokenType::kGreaterOrEqual, Precedence::kLessGreater},
    {TokenType::kPlus, Precedence::kSum},
    {TokenType::kMinus, Precedence::kSum},
    {TokenType::kPercent, Precedence::kSum},
    {TokenType::kAsterisk, Precedence::kProduct},
    {TokenType::kSlash, Precedence::kProduct},
    {TokenType::kPow, Precedence::kPrefix},
    {TokenType::kLParen, Precedence::kCall},
};

/**
 * @brief Set of tokens that indicate the beginning of a block.
 */
const std::set<TokenType> kBlockTypes {
    TokenType::kIf,
    TokenType::kFor,
    TokenType::kWhile,
    TokenType::kFunction,
    TokenType::kElse,
};

/**
 * @brief Represents a parsing error, including the token where it occurred and a descriptive message.
 */
struct ParserError {
    Token token;
    std::string message;
};

/**
 * @class Parser
 * @brief Parses a sequence of tokens into an abstract syntax tree (AST).
 * @details Walks through the sequence of tokens, turning it into an abstract syntax tree for later evaluation.
 * Also, parses literals (integers, strings, null etc.) and puts their value into the corresponding nodes.
 */
class Parser {
public:
    Parser(Lexer& lexer);

    Parser(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser& operator=(Parser&&) = delete;
    ~Parser() = default;

    /**
     * @brief Parses entire program from tokens.
     * @return Top-level Program node containing all statements.
     * 
     * @details This is the entry point for parsing.
     * Example:
     *   x = 5
     *   y = x
     * Would produce a Program node with two child statements.
     */
    Program ParseProgram();

    /**
     * @brief Checks whether a token can be an end of an expression or not.
     * Specifically, checks if type is EOF or a NewLine.
     * @param type The token type to check.
     */
    static bool IsEndOfExpression(TokenType type);
    
    const std::vector<ParserError>& GetErrors() const;

private:
    Lexer& lexer_;
    Token current_token_;
    Token peek_token_;

    std::vector<ParserError> errors_;

    /** @brief Reads next token from the lexer. */
    void AdvanceToken();

    /**
     * @brief Creates a node of the given type and includes current token to it.
     * @return std::unique_ptr with the new node.
     */
    template<typename T>
    std::unique_ptr<T> MakeNode();

    std::unique_ptr<Statement> ParseStatement();
    std::unique_ptr<AssignStatement> ParseAssignStatement();
    std::unique_ptr<ReturnStatement> ParseReturnStatement();
    std::unique_ptr<ExpressionStatement> ParseExpressionStatement();
    std::unique_ptr<BreakStatement> ParseBreakStatement();
    std::unique_ptr<ContinueStatement> ParseContinueStatement();
    std::unique_ptr<WhileStatement> ParseWhileStatement();
    std::unique_ptr<ForStatement> ParseForStatement();

    bool IsCurrentToken(TokenType type) const;
    bool IsPeekToken(TokenType type) const;

    /**
     * @brief Checks whether the current token can be an end of a block statement or not.
     * Specifically, checks if type is TokenType::kEnd or a TokenType::kElse (for if blocks).
     */
    bool IsCurrentTokenEndOfBlock() const;

    /**
     * @brief Checks whether the current token can be an beginning of a block statement or not.
     * @see ItmoScript::kBlockTypes for the set of such tokens.
     */
    bool IsCurrentTokenBlock() const;

    /**
     * @brief Checks if next token is of given type. If it is, reads it.
     * Otherwise, adds an error.
     * @param type Token type to check.
     */
    bool ExpectPeek(TokenType type);

    /**
     * @brief Adds an error message including current token.
     */
    void AddError(const std::string& msg);

    /**
     * @brief Adds an error message that next token is not of expected type.
     */
    void PeekError(TokenType expected_type);

    void AddUnknownTokenError();
    void AddNoPrefixFuncError(TokenType type);

    /**
     * @brief Returns precedence of the next token.
     * @return If no precedence is set for the next token's type, returns Precedence::kLowest.
     * Otherwise, the actual precedence of the next token.
     */
    Precedence PeekPrecedence() const;
    
    /**
     * @brief Returns precedence of the current token.
     * @return If no precedence is set for the current token's type, returns Precedence::kLowest.
     * Otherwise, the actual precedence of the current token.
     */
    Precedence GetCurrentPrecedence() const;

    using PrefixParseFunc = std::function<std::unique_ptr<Expression>(void)>;
    using InfixParseFunc = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

    std::map<TokenType, PrefixParseFunc> prefix_parse_funcs_;
    std::map<TokenType, InfixParseFunc> infix_parse_funcs_;

    /**
     * @brief Parses complex expression with a given precedence (lowest by default).
     * Leaves the current token pointing to the last token of the parsed expression.
     * @param precedence Minimal priority for stop.
     * @return AST node representing the expression.
     * 
     * @details Algorithm follows the "Pratt parsing" principle:<br>
     * 1. First, parse the prefix part (number, identifier, etc.)
     * 2. Then parse infix parts while the current token precedence is higher than the given precedence.
     */
    std::unique_ptr<Expression> ParseExpression(Precedence precedence = Precedence::kLowest);

    std::unique_ptr<PrefixExpression> ParsePrefixExpression();
    std::unique_ptr<InfixExpression> ParseInfixExpression(std::unique_ptr<Expression> left);

    std::unique_ptr<Identifier> ParseIdentifier();
    std::unique_ptr<IntegerLiteral> ParseIntegerLiteral();
    std::unique_ptr<FloatLiteral> ParseFloatLiteral();
    std::unique_ptr<StringLiteral> ParseStringLiteral();
    std::unique_ptr<BooleanLiteral> ParseBooleanLiteral();
    std::unique_ptr<NullTypeLiteral> ParseNullTypeLiteral();
    std::unique_ptr<Expression> ParseGroupedExpression();
    std::unique_ptr<IfExpression> ParseIfExpression();
    std::unique_ptr<BlockStatement> ParseBlockStatement();
    std::unique_ptr<FunctionLiteral> ParseFunctionLiteral();
    std::unique_ptr<CallExpression> ParseCallExpression(std::unique_ptr<Expression> function);

    std::optional<std::vector<std::unique_ptr<Identifier>>> ParseFunctionParameters();
    std::optional<std::vector<std::unique_ptr<Expression>>> ParseCallArguments();

    /**
     * @brief Replaces all recognized escape sequences (e.g., \\n, \t) in the string
     * with their corresponding characters.
     * @param str String containing escape sequences.
     * @return std::nullopt if an unknown escape sequence is encountered,
     * otherwise the resulting unescaped string.
     */
    std::optional<std::string> ProcessEscapeSequences(std::string_view str);
};
    
template<typename T>
std::unique_ptr<T> Parser::MakeNode() {
    return std::make_unique<T>(current_token_);
}

} // namespace ItmoScript
