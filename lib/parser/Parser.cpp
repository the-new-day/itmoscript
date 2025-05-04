#include "Parser.hpp"
#include "utils.hpp"

#include <format>

namespace ItmoScript {

Parser::Parser(Lexer& lexer)
    : lexer_(lexer) 
{
    // set current_token_ and peek_token_
    ReadNextToken();
    ReadNextToken();

    prefix_parse_funcs_[TokenType::kIdentifier] = [this]() { return this->ParseIdentifier(); };
    prefix_parse_funcs_[TokenType::kInt] = [this]() { return this->ParseIntegerLiteral(); };
}

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
    if (IsCurrentToken(TokenType::kIdentifier)) {
        if (IsPeekToken(TokenType::kAssign)) {
            return ParseAssignStatement();
        }
    } else if (IsCurrentToken(TokenType::kReturn)) {
        return ParseReturnStatement();
    } else if (IsCurrentToken(TokenType::kIllegal)) {
        AddUnknownTokenError();
        return nullptr;
    }

    return ParseExpressionStatement();
}

std::unique_ptr<AssignStatement> Parser::ParseAssignStatement() {
    auto statement = std::make_unique<AssignStatement>(current_token_);

    statement->ident = std::make_unique<Identifier>(current_token_);
    statement->ident->name = current_token_.literal;

    // TODO: parse expression
    ReadNextToken();
    ReadNextToken();
    statement->expr = std::make_unique<Expression>(current_token_);

    return statement;
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto statement = std::make_unique<ReturnStatement>(current_token_);
    // TODO: parse expression
    statement->expr = std::make_unique<Expression>();
    ReadNextToken();
    return statement;
}

std::unique_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    auto statement = std::make_unique<ExpressionStatement>(current_token_);
    statement->expr = ParseExpression(Precedence::kLowest);
    return statement;
}

std::unique_ptr<Expression> Parser::ParseExpression(Precedence precedence) {
    // TODO: remove
    if (!prefix_parse_funcs_.contains(current_token_.type)) {
        return nullptr;
    }

    return std::invoke(prefix_parse_funcs_.at(current_token_.type));
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

void Parser::AddError(const std::string& msg) {
    errors_.push_back(msg);
}

void Parser::AddError(std::string&& msg) {
    errors_.push_back(std::move(msg));
}

void Parser::AddUnknownTokenError() {
    AddError(std::format(
        "got unknown token '{}'",
        current_token_.literal
    ));
}

void Parser::PeekError(TokenType expected_type) {
    AddError(std::format(
        "expected next token to be {}, got {} instead",
        kTokenTypeNames.at(expected_type),
        kTokenTypeNames.at(peek_token_.type)
    ));
}

std::unique_ptr<Identifier> Parser::ParseIdentifier() {
    auto ident = std::make_unique<Identifier>(current_token_);
    ident->name = current_token_.literal;
    return ident;
}

std::unique_ptr<IntegerLiteral> Parser::ParseIntegerLiteral() {
    std::optional<int64_t> parsing_result = ParseNumber<int64_t>(current_token_.literal);
    if (!parsing_result.has_value()) {
        AddError(std::format("could not parse {} as an integer", current_token_.literal));
        return nullptr;
    }

    auto int_literal = std::make_unique<IntegerLiteral>(current_token_);
    int_literal->value = parsing_result.value();
    return int_literal;
}

const std::vector<std::string>& Parser::GetErrors() const {
    return errors_;
}

} // namespace ItmoScript
