#include "Parser.hpp"
#include "ParsingError.hpp"
#include "utils.hpp"

#include <format>
#include <string_view>

namespace itmoscript {

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
    prefix_parse_funcs_[TokenType::kNil] = [this]() { return this->ParseNullTypeLiteral(); };
    prefix_parse_funcs_[TokenType::kBang] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kMinus] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kPlus] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kNot] = [this]() { return this->ParsePrefixExpression(); };
    prefix_parse_funcs_[TokenType::kTrue] = [this]() { return this->ParseBooleanLiteral(); };
    prefix_parse_funcs_[TokenType::kFalse] = [this]() { return this->ParseBooleanLiteral(); };
    prefix_parse_funcs_[TokenType::kLParen] = [this]() { return this->ParseGroupedExpression(); };
    prefix_parse_funcs_[TokenType::kIf] = [this]() { return this->ParseIfExpression(); };
    prefix_parse_funcs_[TokenType::kFunction] = [this]() { return this->ParseFunctionLiteral(); };

    auto infix_parser = [this](std::shared_ptr<ast::Expression> expr) {
        return this->ParseInfixExpression(std::move(expr));
    };

    infix_parse_funcs_[TokenType::kPlus] = infix_parser;
    infix_parse_funcs_[TokenType::kMinus] = infix_parser;
    infix_parse_funcs_[TokenType::kSlash] = infix_parser;
    infix_parse_funcs_[TokenType::kAsterisk] = infix_parser;
    infix_parse_funcs_[TokenType::kPow] = infix_parser;
    infix_parse_funcs_[TokenType::kPercent] = infix_parser;
    infix_parse_funcs_[TokenType::kEqual] = infix_parser;
    infix_parse_funcs_[TokenType::kNotEqual] = infix_parser;
    infix_parse_funcs_[TokenType::kLess] = infix_parser;
    infix_parse_funcs_[TokenType::kLessOrEqual] = infix_parser;
    infix_parse_funcs_[TokenType::kGreater] = infix_parser;
    infix_parse_funcs_[TokenType::kGreaterOrEqual] = infix_parser;
    infix_parse_funcs_[TokenType::kAnd] = infix_parser;
    infix_parse_funcs_[TokenType::kOr] = infix_parser;
    infix_parse_funcs_[TokenType::kLParen] = [this](std::shared_ptr<ast::Expression> function) {
        Token token = function->token;
        auto expr = this->ParseCallExpression(std::move(function));

        if (auto ident = dynamic_cast<ast::Identifier*>(expr->function.get())) {
            expr->function_name = ident->name;
        } else if (dynamic_cast<ast::FunctionLiteral*>(expr->function.get()) == nullptr) {
            ThrowError(std::format(
                "Object {} is not callable. This incident will be reported", token.literal
            ));
        }

        return expr;
    };
}

void Parser::AdvanceToken() {
    current_token_ = peek_token_;
    peek_token_ = lexer_.GetNextToken();
}

ast::Program Parser::ParseProgram() {
    ast::Program program;

    while (!IsCurrentToken(TokenType::kEOF)) {
        if (IsCurrentToken(TokenType::kNewLine)) {
            AdvanceToken();
            continue;
        }

        program.AddStatement(ParseStatement());
        AdvanceToken();
    }

    return program;
}

std::shared_ptr<ast::Statement> Parser::ParseStatement() {
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
    }

    return ParseExpressionStatement();
}

std::shared_ptr<ast::AssignStatement> Parser::ParseAssignStatement() {
    auto statement = MakeNode<ast::AssignStatement>();

    statement->ident = MakeNode<ast::Identifier>();
    statement->ident->name = current_token_.literal;

    Consume(TokenType::kAssign);

    AdvanceToken();
    statement->expr = ParseExpression();
    return statement;
}

std::shared_ptr<ast::ReturnStatement> Parser::ParseReturnStatement() {
    auto statement = MakeNode<ast::ReturnStatement>();
    AdvanceToken();

    if (!IsCurrentToken(TokenType::kEOF) && !IsCurrentToken(TokenType::kNewLine)) {
        statement->expr = ParseExpression();
    }

    return statement;
}

std::shared_ptr<ast::ExpressionStatement> Parser::ParseExpressionStatement() {
    auto statement = MakeNode<ast::ExpressionStatement>();
    statement->expr = ParseExpression();
    return statement;
}

std::shared_ptr<ast::Expression> Parser::ParseExpression(Precedence precedence) {
    if (!prefix_parse_funcs_.contains(current_token_.type)) {
        throw lang_exceptions::ParsingError{
            current_token_.line, 
            current_token_.column, 
            std::format(
                "unexpected token '{}'",
                kTokenTypeNames.at(current_token_.type)
            )
        };
    }

    std::shared_ptr<ast::Expression> left = std::invoke(prefix_parse_funcs_.at(current_token_.type));

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

void Parser::Consume(TokenType type) {
    if (IsPeekToken(type)) {
        AdvanceToken();
    } else {
        ThrowPeekError(type);
    }
}

void Parser::ThrowPeekError(TokenType expected_type) {
    throw lang_exceptions::ParsingError{
        current_token_.line, 
        current_token_.column + current_token_.literal.size(),
        std::format(
            "expected next token to be {}, got {} instead",
            kTokenTypeNames.at(expected_type),
            kTokenTypeNames.at(peek_token_.type)
        ),
    };
}

bool Parser::IsEndOfExpression(TokenType type) {
    return type == TokenType::kNewLine 
        || type == TokenType::kEOF;
}

bool Parser::IsCurrentTokenEndOfBlock() const {
    return IsCurrentToken(TokenType::kEnd)
        || IsCurrentToken(TokenType::kElse)
        || IsCurrentToken(TokenType::kElseIf);
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

std::shared_ptr<ast::Identifier> Parser::ParseIdentifier() {
    auto ident = MakeNode<ast::Identifier>();
    ident->name = current_token_.literal;
    return ident;
}

std::shared_ptr<ast::IntegerLiteral> Parser::ParseIntegerLiteral() {
    // TODO: parsing with base other than 10
    std::expected<int64_t, std::errc> parsing_result = utils::ParseNumber<int64_t>(current_token_.literal);

    if (!parsing_result.has_value()) {
        const std::errc& error = parsing_result.error();
        if (error == std::errc::result_out_of_range) {
            ThrowError(std::format("could not parse {} as an Integer: result is out of range", current_token_.literal));
        } else {
            ThrowError(std::format("could not parse {} as an Integer", current_token_.literal));
        }
    }

    auto int_literal = MakeNode<ast::IntegerLiteral>();
    int_literal->value = parsing_result.value();
    return int_literal;
}

std::shared_ptr<ast::FloatLiteral> Parser::ParseFloatLiteral() {
    std::expected<double, std::errc> parsing_result = utils::ParseNumber<double>(current_token_.literal);

    if (!parsing_result.has_value()) {
        const std::errc& error = parsing_result.error();
        if (error == std::errc::result_out_of_range) {
            ThrowError(std::format("could not parse {} as a Float: result is out of range", current_token_.literal));
        } else {
            ThrowError(std::format("could not parse {} as a Float", current_token_.literal));
        }
    }

    auto float_literal = MakeNode<ast::FloatLiteral>();
    float_literal->value = parsing_result.value();
    return float_literal;
}

std::shared_ptr<ast::StringLiteral> Parser::ParseStringLiteral() {
    auto string_literal = MakeNode<ast::StringLiteral>();

    std::string_view literal = string_literal->token.literal;
    literal = literal.substr(1, literal.size() - 2);

    string_literal->value = ProcessEscapeSequences(literal);
    return string_literal;
}

std::shared_ptr<ast::BooleanLiteral> Parser::ParseBooleanLiteral() {
    auto bool_literal = MakeNode<ast::BooleanLiteral>();
    bool_literal->value = IsCurrentToken(TokenType::kTrue);
    return bool_literal;
}

std::shared_ptr<ast::NullTypeLiteral> Parser::ParseNullTypeLiteral() {
    return MakeNode<ast::NullTypeLiteral>();
}

std::shared_ptr<ast::PrefixExpression> Parser::ParsePrefixExpression() {
    auto expr = MakeNode<ast::PrefixExpression>();
    expr->oper = current_token_.literal;
    AdvanceToken();

    expr->right = ParseExpression(Precedence::kPrefix);
    return expr;
}

std::shared_ptr<ast::InfixExpression> Parser::ParseInfixExpression(std::shared_ptr<ast::Expression> left) {
    auto infix_expr = MakeNode<ast::InfixExpression>();
    infix_expr->oper = current_token_.literal;
    infix_expr->left = std::move(left);

    Precedence precedence = GetCurrentPrecedence();
    AdvanceToken();
    infix_expr->right = ParseExpression(precedence);

    return infix_expr;
}

std::shared_ptr<ast::Expression> Parser::ParseGroupedExpression() {
    AdvanceToken();
    auto expr = ParseExpression();
    Consume(TokenType::kRParen);
    return expr;
}

std::shared_ptr<ast::BlockStatement> Parser::ParseBlockStatement() {
    auto block = MakeNode<ast::BlockStatement>();
    AdvanceToken();

    while (!IsCurrentTokenEndOfBlock()) {
        if (IsCurrentToken(TokenType::kNewLine)) {
            AdvanceToken();
            continue;
        } else if (IsCurrentToken(TokenType::kEOF)) {
            ThrowError("unexpected end of block");
        }

        block->AddStatement(ParseStatement());
        AdvanceToken();
    }

    // don't check for 'end' token here, because of 'if' expressions
    return block;
}

std::shared_ptr<ast::IfExpression> Parser::ParseIfExpression() {
    auto expr = MakeNode<ast::IfExpression>();
    AdvanceToken();

    expr->alternatives.resize(1);
    expr->alternatives[0].condition = ParseExpression();
    Consume(TokenType::kThen);

    expr->alternatives[0].consequence = ParseBlockStatement();

    bool has_final_else = false;

    while (IsCurrentToken(TokenType::kElse) || IsCurrentToken(TokenType::kElseIf)) {
        if (has_final_else) {
            ThrowError("unexpected branch beginning");
        }

        ast::IfBranch branch{current_token_};

        if (IsCurrentToken(TokenType::kElseIf)) {
            AdvanceToken();
            branch.condition = ParseExpression(Precedence::kLowest);
            Consume(TokenType::kThen);
        } else {
            has_final_else = true;
        }

        branch.consequence = ParseBlockStatement();
        expr->alternatives.push_back(std::move(branch));
    }

    if (!IsCurrentToken(TokenType::kEnd)) {
        ThrowError("unexpected end of block");
    }

    Consume(TokenType::kIf);
    return expr;
}

std::shared_ptr<ast::FunctionLiteral> Parser::ParseFunctionLiteral() {
    auto function_lit = MakeNode<ast::FunctionLiteral>();
    Consume(TokenType::kLParen);

    function_lit->parameters = ParseFunctionParameters();
    function_lit->body = ParseBlockStatement();

    Consume(TokenType::kFunction);
    return function_lit;
}

std::shared_ptr<ast::CallExpression> Parser::ParseCallExpression(std::shared_ptr<ast::Expression> function) {
    auto expr = MakeNode<ast::CallExpression>();
    expr->function = std::move(function);
    expr->arguments = ParseCallArguments();
    return expr;
}

std::vector<std::shared_ptr<ast::Expression>> Parser::ParseCallArguments() {
    std::vector<std::shared_ptr<ast::Expression>> args;
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

    Consume(TokenType::kRParen);
    return args;
}

std::shared_ptr<ast::WhileStatement> Parser::ParseWhileStatement() {
    auto stmt = MakeNode<ast::WhileStatement>();
    AdvanceToken();

    stmt->condition = ParseExpression();
    stmt->body = ParseBlockStatement();

    Consume(TokenType::kWhile);
    return stmt;
}

std::shared_ptr<ast::ForStatement> Parser::ParseForStatement() {
    auto stmt = MakeNode<ast::ForStatement>();
    Consume(TokenType::kIdentifier);

    stmt->iter = ParseIdentifier();
    
    Consume(TokenType::kIn);
    AdvanceToken();
    
    stmt->range = ParseExpression();
    stmt->body = ParseBlockStatement();

    Consume(TokenType::kFor);
    return stmt;
}

std::shared_ptr<ast::BreakStatement> Parser::ParseBreakStatement() {
    auto stmt = MakeNode<ast::BreakStatement>();
    return stmt;
}

std::shared_ptr<ast::ContinueStatement> Parser::ParseContinueStatement() {
    auto stmt = MakeNode<ast::ContinueStatement>();
    return stmt;
}

std::vector<std::shared_ptr<ast::Identifier>> Parser::ParseFunctionParameters() {
    std::vector<std::shared_ptr<ast::Identifier>> identifiers;

    if (IsPeekToken(TokenType::kRParen)) {
        AdvanceToken();
        return identifiers;
    }

    Consume(TokenType::kIdentifier);

    auto ident = ParseIdentifier();
    identifiers.push_back(std::move(ident));

    while (IsPeekToken(TokenType::kComma)) {
        if (!IsCurrentToken(TokenType::kIdentifier)) {
            ThrowPeekError(TokenType::kIdentifier);
        }

        AdvanceToken();
        Consume(TokenType::kIdentifier);

        ident = ParseIdentifier();
        identifiers.push_back(std::move(ident));
    }

    Consume(TokenType::kRParen);
    return identifiers;
}

std::string Parser::ProcessEscapeSequences(std::string_view str) {
    std::string result;
    result.reserve(str.size());

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != '\\') {
            result += str[i];
            continue;
        } else if (i == str.size() - 1) {
            ThrowError("unexpected end of a string");
        }

        switch (str[i + 1]) {
            case '\'':
                result += '\'';
                break;
            case '"':
                result += '\"';
                break;
            case '?':
                result += '\?';
                break;
            case '\\':
                result += '\\';
                break;
            case 'a':
                result += '\a';
                break;
            case 'b':
                result += '\b';
                break;
            case 'f':
                result += '\f';
                break;
            case 'n':
                result += '\n';
                break;
            case 'r':
                result += '\r';
                break;
            case 't':
                result += '\t';
                break;
            case 'v':
                result += '\v';
                break;
            default:
                ThrowError("unknown escape sequence");
        }

        ++i;
    }

    return result;
}

void Parser::ThrowError(const std::string& message) noexcept(false) {
    throw lang_exceptions::ParsingError{current_token_.line, current_token_.column, message};
}

} // namespace itmoscript
