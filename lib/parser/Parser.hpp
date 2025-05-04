#pragma once

#include "lexer/Lexer.hpp"
#include "ast/AST.hpp"

#include <optional>

namespace ItmoScript {

class Parser {
public:
    Parser(Lexer& lexer)
        : lexer_(lexer) 
    {
        // set current_token_ and peek_token_
        ReadNextToken();
        ReadNextToken();
    }

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

    bool IsCurrentToken(TokenType type) const;
    bool IsPeekToken(TokenType type) const;
    bool ExpectPeek(TokenType type);

    void AddUnknownTokenError();
    void PeekError(TokenType expected_type);
};
    
} // namespace ItmoScript

