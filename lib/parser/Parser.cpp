#include "Parser.hpp"

#include <format>

namespace ItmoScript {
 
void Parser::ReadNextToken() {
    current_token_ = peek_token_;
    peek_token_ = lexer_.GetNextToken();
}

Program Parser::ParseProgram() {
    Program program;

    while (!IsCurrentToken(TokenType::kEOF)) {
        auto statement = ParseStatement();
        if (statement != nullptr) {
            program.AddStatement(std::move(statement));
        }

        ReadNextToken();
    }

    return program;
}

std::unique_ptr<Statement> Parser::ParseStatement() {
    if (current_token_.type == TokenType::kIdentifier) {
        if (peek_token_.type == TokenType::kAssign) {
            return ParseAssignStatement();
        } else {
            PeekError(TokenType::kAssign);
        }
    } else if (current_token_.type == TokenType::kReturn) {
        return ParseReturnStatement();
    }

    return nullptr;
}

std::unique_ptr<AssignStatement> Parser::ParseAssignStatement() {
    auto statement = std::make_unique<AssignStatement>(current_token_);

    statement->ident = std::make_unique<Identifier>(current_token_);
    statement->ident->value = current_token_.literal;
    statement->ident->token = current_token_;

    // if (!ExpectPeek(TokenType::kAssign)) {
    //     PeekError(TokenType::kAssign);
    //     return nullptr;
    // }

    // TODO: parse expression
    ReadNextToken();

    return statement;
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto statement = std::make_unique<ReturnStatement>(current_token_);
    // TODO: parse expression
    ReadNextToken();
    return statement;
}

bool Parser::IsCurrentToken(TokenType type) const {
    return current_token_.type == type;
}

bool Parser::IsPeekToken(TokenType type) const {
    return peek_token_.type == type;
}

bool Parser::ExpectPeek(TokenType type) {
    if (IsPeekToken(type)) {
        ReadNextToken();
        return true;
    } else {
        PeekError(type);
        return false;
    }
}

void Parser::AddUnknownTokenError() {
    errors_.push_back(std::format(
        "got unknown token '{}'",
        current_token_.literal
    ));
}

void Parser::PeekError(TokenType expected_type) {
    errors_.push_back(std::format(
        "expected next token to be {}, got {} instead",
        kTokenTypeNames.at(expected_type),
        kTokenTypeNames.at(peek_token_.type)
    ));
}

const std::vector<std::string>& Parser::GetErrors() const {
    return errors_;
}

} // namespace ItmoScript
