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
    prefix_parse_funcs_[TokenType::kFloat] = [this]() { return this->ParseFloatLiteral(); };
    prefix_parse_funcs_[TokenType::kStringLiteral] = [this]() { return this->ParseStringLiteral(); };
    prefix_parse_funcs_[TokenType::kBang] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kMinus] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kTrue] = [this]() { return this->ParseBooleanLiteral(); };
    prefix_parse_funcs_[TokenType::kFalse] = [this]() { return this->ParseBooleanLiteral(); };
    prefix_parse_funcs_[TokenType::kLParen] = [this]() { return this->ParseGroupedExpression(); };
    prefix_parse_funcs_[TokenType::kIf] = [this]() { return this->ParseIfExpression(); };
    prefix_parse_funcs_[TokenType::kFunction] = [this]() { return this->ParseFunctionLiteral(); };

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
    infix_parse_funcs_[TokenType::kLParen] = [this](std::unique_ptr<Expression> function) { 
        return this->ParseCallExpression(std::move(function)); 
    };
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
    } else if (IsCurrentToken(TokenType::kWhile)) {
        return ParseWhileStatement();
    } else if (IsCurrentToken(TokenType::kFor)) {
        return ParseForStatement();
    } else if (IsCurrentToken(TokenType::kBreak)) {
        return ParseBreakStatement();
    } else if (IsCurrentToken(TokenType::kContinue)) {
        return ParseContinueStatement();
    } else if (IsCurrentToken(TokenType::kIllegal)) {
        AddUnknownTokenError();
        return nullptr;
    }

    return ParseExpressionStatement();
}

std::unique_ptr<AssignStatement> Parser::ParseAssignStatement() {
    auto statement = MakeNode<AssignStatement>();

    statement->ident = MakeNode<Identifier>();
    statement->ident->name = current_token_.literal;

    if (!ExpectPeek(TokenType::kAssign)) {
        return nullptr;
    }

    AdvanceToken();
    statement->expr = ParseExpression();
    return statement;
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto statement = MakeNode<ReturnStatement>();
    AdvanceToken();
    statement->expr = ParseExpression();
    return statement;
}

std::unique_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    auto statement = MakeNode<ExpressionStatement>();
    statement->expr = ParseExpression();
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
    errors_.push_back({.token = current_token_, .message = msg});
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
        "unexpected token '{}'",
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
    auto ident = MakeNode<Identifier>();
    ident->name = current_token_.literal;
    return ident;
}

std::unique_ptr<IntegerLiteral> Parser::ParseIntegerLiteral() {
    std::optional<int64_t> parsing_result = ParseNumber<int64_t>(current_token_.literal);
    if (!parsing_result.has_value()) {
        AddError(std::format("could not parse {} as an integer", current_token_.literal));
        return nullptr;
    }

    auto int_literal = MakeNode<IntegerLiteral>();
    int_literal->value = parsing_result.value();
    return int_literal;
}

std::unique_ptr<FloatLiteral> Parser::ParseFloatLiteral() {
    std::optional<double> parsing_result = ParseNumber<double>(current_token_.literal);
    if (!parsing_result.has_value()) {
        AddError(std::format("could not parse {} as a float", current_token_.literal));
        return nullptr;
    }

    auto float_literal = MakeNode<FloatLiteral>();
    float_literal->value = parsing_result.value();
    return float_literal;
}

std::unique_ptr<StringLiteral> Parser::ParseStringLiteral() {
    auto string_literal = MakeNode<StringLiteral>();
    const std::string& literal = string_literal->token.literal;
    string_literal->value = literal.substr(1, literal.size() - 2);
    return string_literal;
}

std::unique_ptr<BooleanLiteral> Parser::ParseBooleanLiteral() {
    auto bool_literal = MakeNode<BooleanLiteral>();
    bool_literal->value = IsCurrentToken(TokenType::kTrue);
    return bool_literal;
}

std::unique_ptr<PrefixExpression> Parser::ParsePrefixExpression() {
    auto expr = MakeNode<PrefixExpression>();
    expr->oper = current_token_.literal;
    AdvanceToken();

    expr->right = ParseExpression(Precedence::kPrefix);
    return expr;
}

std::unique_ptr<InfixExpression> Parser::ParseInfixExpression(std::unique_ptr<Expression> left) {
    auto infix_expr = MakeNode<InfixExpression>();
    infix_expr->oper = current_token_.literal;
    infix_expr->left = std::move(left);

    Precedence precedence = GetCurrentPrecedence();
    AdvanceToken();
    infix_expr->right = ParseExpression(precedence);

    return infix_expr;
}

std::unique_ptr<Expression> Parser::ParseGroupedExpression() {
    AdvanceToken();
    auto expr = ParseExpression();

    if (ExpectPeek(TokenType::kRParen)) {
        return expr;
    } else {
        return nullptr;
    }
}

std::unique_ptr<BlockStatement> Parser::ParseBlockStatement() {
    auto block = MakeNode<BlockStatement>();
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

std::unique_ptr<IfExpression> Parser::ParseIfExpression() {
    auto expr = MakeNode<IfExpression>();
    AdvanceToken();

    expr->main_condition = ParseExpression();
    if (expr->main_condition == nullptr) {
        return nullptr;
    }

    if (!ExpectPeek(TokenType::kThen)) {
        return nullptr;
    }

    expr->main_consequence = ParseBlockStatement();
    if (expr->main_consequence == nullptr) {
        return nullptr;
    }

    while (IsCurrentToken(TokenType::kElse)) {
        AdvanceToken();
        IfBranch branch{current_token_};

        if (IsCurrentToken(TokenType::kIf)) {
            AdvanceToken();
            branch.condition = ParseExpression(Precedence::kLowest);
            if (branch.condition == nullptr || !ExpectPeek(TokenType::kThen)) {
                return nullptr;
            }
        }

        branch.consequence = ParseBlockStatement();
        if (branch.consequence == nullptr) {
            return nullptr;
        }

        expr->alternatives.push_back(std::move(branch));
    }

    if (!IsCurrentToken(TokenType::kEnd) || !ExpectPeek(TokenType::kIf)) {
        return nullptr;
    }

    return expr;
}

std::unique_ptr<FunctionLiteral> Parser::ParseFunctionLiteral() {
    auto function_lit = MakeNode<FunctionLiteral>();

    if (!ExpectPeek(TokenType::kLParen)) {
        return nullptr;
    }

    auto params = ParseFunctionParameters();
    if (!params.has_value()) {
        return nullptr;
    }

    function_lit->parameters = std::move(params.value());
    function_lit->body = ParseBlockStatement();
    if (function_lit->body == nullptr) {
        return nullptr;
    }

    if (!ExpectPeek(TokenType::kFunction)) {
        return nullptr;
    }

    return function_lit;
}

std::unique_ptr<CallExpression> Parser::ParseCallExpression(std::unique_ptr<Expression> function) {
    auto expr = MakeNode<CallExpression>();
    expr->function = std::move(function);
    auto args = ParseCallArguments();

    if (args.has_value()) {
        expr->arguments = std::move(args.value());
        return expr;
    } else {
        return nullptr;
    }
}

std::optional<std::vector<std::unique_ptr<Expression>>> Parser::ParseCallArguments() {
    std::vector<std::unique_ptr<Expression>> args;
    AdvanceToken();

    if (IsCurrentToken(TokenType::kRParen)) {
        return args;
    }
    
    args.push_back(ParseExpression());

    while (IsPeekToken(TokenType::kComma)) {
        AdvanceToken();
        AdvanceToken();

        args.push_back(ParseExpression());
    }

    if (!ExpectPeek(TokenType::kRParen)) {
        return std::nullopt;
    }

    return args;
}

std::unique_ptr<WhileStatement> Parser::ParseWhileStatement() {
    auto stmt = MakeNode<WhileStatement>();
    AdvanceToken();

    stmt->condition = ParseExpression();
    if (stmt->condition == nullptr) {
        return nullptr;
    }

    stmt->body = ParseBlockStatement();
    if (stmt->body == nullptr) {
        return nullptr;
    }

    if (!ExpectPeek(TokenType::kWhile)) {
        return nullptr;
    }

    return stmt;
}

std::unique_ptr<ForStatement> Parser::ParseForStatement() {
    auto stmt = MakeNode<ForStatement>();

    if (!ExpectPeek(TokenType::kIdentifier)) {
        return nullptr;
    }

    stmt->iter = ParseIdentifier();
    
    if (!ExpectPeek(TokenType::kIn)) {
        return nullptr;
    }

    AdvanceToken();
    
    stmt->range = ParseExpression();
    if (stmt->range == nullptr) {
        return nullptr;
    }

    stmt->body = ParseBlockStatement();
    if (stmt->body == nullptr) {
        return nullptr;
    }

    if (!ExpectPeek(TokenType::kFor)) {
        return nullptr;
    }

    return stmt;
}

std::unique_ptr<BreakStatement> Parser::ParseBreakStatement() {
    auto stmt = MakeNode<BreakStatement>();
    AdvanceToken();
    return stmt;
}

std::unique_ptr<ContinueStatement> Parser::ParseContinueStatement() {
    auto stmt = MakeNode<ContinueStatement>();
    AdvanceToken();
    return stmt;
}

std::optional<std::vector<std::unique_ptr<Identifier>>> Parser::ParseFunctionParameters() {
    std::vector<std::unique_ptr<Identifier>> identifiers;

    if (IsPeekToken(TokenType::kRParen)) {
        AdvanceToken();
        return identifiers;
    }

    if (!ExpectPeek(TokenType::kIdentifier)) {
        return std::nullopt;
    }

    auto ident = ParseIdentifier();
    identifiers.push_back(std::move(ident));

    while (IsPeekToken(TokenType::kComma)) {
        if (!IsCurrentToken(TokenType::kIdentifier)) {
            PeekError(TokenType::kIdentifier);
            return std::nullopt;
        }

        AdvanceToken();
        AdvanceToken();

        ident = ParseIdentifier();
        identifiers.push_back(std::move(ident));
    }

    if (!ExpectPeek(TokenType::kRParen)) {
        return std::nullopt;
    }

    return identifiers;
}

const std::vector<ParserError>& Parser::GetErrors() const {
    return errors_;
}

} // namespace ItmoScript
