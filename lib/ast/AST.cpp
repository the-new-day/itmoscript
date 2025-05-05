#include "AST.hpp"

#include <format>

namespace ItmoScript {

std::string Program::GetTokenLiteral() const {
    if (!statements_.empty()) {
        return statements_[0]->token.literal;
    }

    return std::string{};
}

const std::vector<std::unique_ptr<Statement>>& Program::GetStatements() const {
    return statements_;
}

void Program::AddStatement(std::unique_ptr<Statement> statement) {
    statements_.push_back(std::move(statement));
}

std::string Program::String() const {
    std::string result;

    for (size_t i = 0; i < statements_.size(); ++i) {
        result += statements_[i]->String();
        if (i != statements_.size() - 1)
            result += '\n';
    }

    return result;
}

std::string AssignStatement::String() const {
    return std::format("{} = {}", ident->name, expr->String());
}

std::string ReturnStatement::String() const {
    return std::format("return {}", expr->String());
}

std::string ExpressionStatement::String() const {
    return expr->String();
}

std::string Identifier::String() const {
    return name;
}

std::string Statement::String() const {
    return token.literal;
}

std::string Expression::String() const {
    return token.literal;
}

std::string PrefixExpression::String() const {
    return std::format("({}{})", oper, right->String());
}

std::string InfixExpression::String() const {
    return std::format("({} {} {})", left->String(), oper, right->String());
}

std::string IntegerLiteral::String() const {
    return token.literal;
}

std::string BooleanLiteral::String() const {
    return token.literal;
}

} // namespace ItmoScript
