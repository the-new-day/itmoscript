#include "Parser.hpp"
#include "utils.hpp"

#include <format>

namespace ItmoScript {

Parser::Parser(Lexer& lexer)
    : lexer_(lexer) 
{
    // set current_token_ and peek_token_
    AdvanceToken();
    AdvanceToken();

    prefix_parse_funcs_[TokenType::kIdentifier] = [this]() { return this->ParseIdentifier(); };
    prefix_parse_funcs_[TokenType::kInt] = [this]() { return this->ParseIntegerLiteral(); };
    prefix_parse_funcs_[TokenType::kBang] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kMinus] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kTrue] = [this]() { return this->ParseBooleanLiteral(); };
    prefix_parse_funcs_[TokenType::kFalse] = [this]() { return this->ParseBooleanLiteral(); };
    prefix_parse_funcs_[TokenType::kLParen] = [this]() { return this->ParseGroupedExpression(); };
    prefix_parse_funcs_[TokenType::kIf] = [this]() { return this->ParseIfExpression(); };

    auto infix_parser = [this](std::unique_ptr<Expression> expr) {
        return this->ParseInfixExpression(std::move(expr));
    };

    infix_parse_funcs_[TokenType::kPlus] = infix_parser;
    infix_parse_funcs_[TokenType::kMinus] = infix_parser;
    infix_parse_funcs_[TokenType::kSlash] = infix_parser;
    infix_parse_funcs_[TokenType::kAsterisk] = infix_parser;
    infix_parse_funcs_[TokenType::kEqual] = infix_parser;
    infix_parse_funcs_[TokenType::kNotEqual] = infix_parser;
    infix_parse_funcs_[TokenType::kLess] = infix_parser;
    infix_parse_funcs_[TokenType::kGreater] = infix_parser;
}

void Parser::AdvanceToken() {
    current_token_ = peek_token_;
    peek_token_ = lexer_.GetNextToken();
}

Program Parser::ParseProgram() {
    Program program;

    while (!IsCurrentToken(TokenType::kEOF)) {
        if (IsCurrentToken(TokenType::kNewLine)) {
            AdvanceToken();
            continue;
        } else if (IsCurrentToken(TokenType::kIllegal)) {
            AddUnknownTokenError();
            AdvanceToken();
            continue;
        }

        auto statement = ParseStatement();
        if (statement != nullptr) {
            program.AddStatement(std::move(statement));
        }

        AdvanceToken();
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
    AdvanceToken();
    AdvanceToken();
    statement->expr = std::make_unique<Expression>(current_token_);

    return statement;
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto statement = std::make_unique<ReturnStatement>(current_token_);
    // TODO: parse expression
    statement->expr = std::make_unique<Expression>();
    AdvanceToken();
    return statement;
}

std::unique_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    auto statement = std::make_unique<ExpressionStatement>(current_token_);
    statement->expr = ParseExpression(Precedence::kLowest);
    return statement;
}

std::unique_ptr<Expression> Parser::ParseExpression(Precedence precedence) {
    if (!prefix_parse_funcs_.contains(current_token_.type)) {
        AddNoPrefixFuncError(current_token_.type);
        return nullptr;
    }

    std::unique_ptr<Expression> left = std::invoke(prefix_parse_funcs_.at(current_token_.type));

    while (!IsEndOfExpression(peek_token_.type) && precedence < PeekPrecedence()) {
        if (!infix_parse_funcs_.contains(peek_token_.type)) {
            return left;
        }

        AdvanceToken();
        left = std::invoke(infix_parse_funcs_.at(current_token_.type), std::move(left));
    }

    return left;
}

bool Parser::IsCurrentToken(TokenType type) const {
    return current_token_.type == type;
}

bool Parser::IsCurrentTokenBlock() const {
    return kBlockTypes.contains(current_token_.type);
}

bool Parser::IsPeekToken(TokenType type) const {
    return peek_token_.type == type;
}

bool Parser::ExpectPeek(TokenType type) {
    if (IsPeekToken(type)) {
        AdvanceToken();
        return true;
    } else {
        PeekError(type);
        return false;
    }
}

void Parser::AddError(const std::string& msg) {
    errors_.push_back(
        std::format("Ln {}, Col {}: {}", current_token_.line, current_token_.column, msg)
    );
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

void Parser::AddNoPrefixFuncError(TokenType type) {
    AddError(std::format(
        "no prefix parse function for {} found",
        kTokenTypeNames.at(type)
    ));
}

bool Parser::IsEndOfExpression(TokenType type) {
    return type == TokenType::kNewLine 
        || type == TokenType::kEOF;
}

bool Parser::IsCurrentTokenEndOfBlock() const {
    return IsCurrentToken(TokenType::kEnd)
        || IsCurrentToken(TokenType::kElse);
}

Precedence Parser::PeekPrecedence() const {
    if (kPrecedences.contains(peek_token_.type)) {
        return kPrecedences.at(peek_token_.type);
    } else {
        return Precedence::kLowest;
    }
}

Precedence Parser::GetCurrentPrecedence() const {
    if (kPrecedences.contains(current_token_.type)) {
        return kPrecedences.at(current_token_.type);
    } else {
        return Precedence::kLowest;
    }
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

std::unique_ptr<BooleanLiteral> Parser::ParseBooleanLiteral() {
    auto bool_literal = std::make_unique<BooleanLiteral>(current_token_);
    bool_literal->value = IsCurrentToken(TokenType::kTrue);
    return bool_literal;
}

std::unique_ptr<PrefixExpression> Parser::ParsePrefixExpression() {
    auto expr = std::make_unique<PrefixExpression>(current_token_);
    expr->oper = current_token_.literal;
    AdvanceToken();

    expr->right = ParseExpression(Precedence::kPrefix);
    return expr;
}

std::unique_ptr<InfixExpression> Parser::ParseInfixExpression(std::unique_ptr<Expression> left) {
    auto infix_expr = std::make_unique<InfixExpression>(current_token_);
    infix_expr->oper = current_token_.literal;
    infix_expr->left = std::move(left);

    Precedence precedence = GetCurrentPrecedence();
    AdvanceToken();
    infix_expr->right = ParseExpression(precedence);

    return infix_expr;
}

std::unique_ptr<Expression> Parser::ParseGroupedExpression() {
    AdvanceToken();
    auto expr = ParseExpression(Precedence::kLowest);

    if (ExpectPeek(TokenType::kRParen)) {
        return expr;
    } else {
        return nullptr;
    }
}

std::unique_ptr<IfExpression> Parser::ParseIfExpression() {
    auto expr = std::make_unique<IfExpression>(current_token_);
    AdvanceToken();

    expr->condition = ParseExpression(Precedence::kLowest);
    if (expr->condition == nullptr) {
        return nullptr;
    }

    if (!ExpectPeek(TokenType::kThen)) {
        return nullptr;
    }

    expr->consequence = ParseBlockStatement();
    if (expr->consequence == nullptr) {
        return nullptr;
    }

    if (IsCurrentToken(TokenType::kElse)) {
        expr->alternative = ParseBlockStatement();
        if (expr->alternative == nullptr) {
            return nullptr;
        }
    }

    if (!ExpectPeek(TokenType::kIf)) {
        return nullptr;
    }

    return expr;
}

std::unique_ptr<BlockStatement> Parser::ParseBlockStatement() {
    auto block = std::make_unique<BlockStatement>(current_token_);
    AdvanceToken();

    while (!IsCurrentTokenEndOfBlock() && !IsCurrentToken(TokenType::kEOF)) {
        if (IsCurrentToken(TokenType::kNewLine)) {
            AdvanceToken();
            continue;
        }

        auto statement = ParseStatement();
        if (statement != nullptr) {
            block->AddStatement(std::move(statement));
        }

        AdvanceToken();
    }

    return block;
}

const std::vector<std::string>& Parser::GetErrors() const {
    return errors_;
}

} // namespace ItmoScript
