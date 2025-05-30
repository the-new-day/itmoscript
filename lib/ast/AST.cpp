#include "AST.hpp"
#include "utils.hpp"

#include <format>

namespace itmoscript {

namespace ast {

const std::vector<std::shared_ptr<Statement>>& Program::GetStatements() const {
    return statements_;
}

void Program::AddStatement(std::shared_ptr<Statement> statement) {
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

const std::vector<std::shared_ptr<Statement>>& BlockStatement::GetStatements() const {
    return statements_;
}

void BlockStatement::AddStatement(std::shared_ptr<Statement> statement) {
    statements_.push_back(std::move(statement));
}

std::string BlockStatement::String() const {
    std::string result;

    // TODO: use utils::Join here and everywhere below

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

std::string OperatorAssignStatement::String() const {
    return std::format(
        "{} {} {}", 
        ident->name, 
        kTokenTypeNames.at(oper), 
        expr->String()
    );
}

std::string ReturnStatement::String() const {
    return std::format("return {}", (expr ? expr->String() : ""));
}

std::string ExpressionStatement::String() const {
    return expr->String();
}

std::string Identifier::String() const {
    return name;
}

std::string PrefixExpression::String() const {
    return std::format("({}{})", kTokenTypeNames.at(oper), right->String());
}

std::string InfixExpression::String() const {
    return std::format("({} {} {})", left->String(), kTokenTypeNames.at(oper), right->String());
}

std::string IfBranch::String() const {
    std::string result;
    if (condition != nullptr) {
        result += condition->String();
        result += " then ";
    }
    
    result += consequence->String();
    return result;
}

std::string IfExpression::String() const {
    std::string result;
    result += " if ";
    result += alternatives[0].condition->String();
    result += " then ";
    result += alternatives[0].consequence->String();

    for (size_t i = 1; i < alternatives.size(); ++i) {
        if (alternatives[i].condition == nullptr) {
            result += " else ";
        } else {
            result += " elseif ";
        }
        result += alternatives[i].String();
    }

    result += " end if";
    return result;
}

std::string FunctionLiteral::String() const {
    std::string result;
    result += token.literal;
    result += "(";

    for (size_t i = 0; i < parameters.size(); ++i) {
        result += parameters[i]->String();
        if (i != parameters.size() - 1) result += ", ";
    }

    result += ") ";
    result += body->String();
    result += " end function";
    return result;
}

std::string ListLiteral::String() const {
    std::string result;
    result += '[';

    for (size_t i = 0; i < elements.size(); ++i) {
        result += elements[i]->String();
        if (i != elements.size() - 1) result += ", ";
    }

    result += ']';
    return result;
}

std::string IndexOperatorExpression::String() const {
    std::string result;
    result += operand->String();
    result += '[';
    result += index ? index->String() : "";

    if (is_slice) {
        result += ':';
        result += second_index ? second_index->String() : "";
    }

    result += ']';
    return result;
}

std::string CallExpression::String() const {
    std::string result;
    result += function->String();
    result += "(";

    for (size_t i = 0; i < arguments.size(); ++i) {
        result += arguments[i]->String();
        if (i != arguments.size() - 1) result += ", ";
    }

    result += ")";
    return result;
}

std::string WhileStatement::String() const {
    std::string result;
    result += "while ";
    result += condition->String();
    result += ' ';

    const auto& stmts = body->GetStatements();

    for (const auto& stmt : body->GetStatements()) {
        result += stmt->String();
        result += ' ';
    }

    result += " end while";
    return result;
}

std::string ForStatement::String() const {
    std::string result;
    result += "for ";
    result += iter->String();
    result += " in ";
    result += range->String();

    const auto& stmts = body->GetStatements();

    for (const auto& stmt : body->GetStatements()) {
        result += stmt->String();
        result += ' ';
    }

    result += " end for";
    return result;
}
    
} // namespace ast

} // namespace itmoscript
