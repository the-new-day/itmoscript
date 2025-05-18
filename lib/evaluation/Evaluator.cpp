#include "Evaluator.hpp"
#include "utils.hpp"

#include <cmath>

namespace ItmoScript {

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

Value Evaluator::Eval(Node& node) {
    node.Accept(*this);
    return result_;
}

void Evaluator::EvalPrefixExpression(const std::string& oper, const Value& right) {
    // TODO: make map with handlers (?)
    if (oper == "!") {
        EvalBangOperatorExpression(right);
    } else if (oper == "-") {
        EvalUnaryMinusOperatorExpression(right);
    } else if (oper == "+") {
        EvalUnaryPlusOperatorExpression(right);
    } else {
        result_ = NullType{}; // TODO: error
    }
}

void Evaluator::EvalBangOperatorExpression(const Value& right) {
    result_ = !right.IsTruphy();
}

void Evaluator::EvalUnaryMinusOperatorExpression(const Value& right) {
    if (right.IsInt()) {
        result_ = -right.GetValue<Int>();
    } else if (right.IsFloat()) {
        result_ = -right.GetValue<Float>();
    } else {
        result_ = NullType{}; // TODO: error
    }
}

void Evaluator::EvalUnaryPlusOperatorExpression(const Value& right) {
    if (right.IsInt()) {
        result_ = right.GetValue<Int>();
    } else if (right.IsFloat()) {
        result_ = right.GetValue<Float>();
    } else {
        result_ = NullType{}; // TODO: error
    }

    // TODO: maybe some other types are acceptable
}

void Evaluator::EvalInfixExpression(const std::string& oper, const Value& left, const Value& right) {
    if (left.IsOfType<Int>() && right.IsOfType<Int>()) {
        EvalIntInfixExpression(oper, left, right);
    } else {
        result_ = NullType{}; // TODO: error
    }
}

void Evaluator::EvalIntInfixExpression(const std::string& oper, const Value& left, const Value& right) {
    Int left_val = left.GetValue<Int>();
    Int right_val = right.GetValue<Int>();

    // TODO: map for operators
    // TODO: error check for division
    // TODO: negative exponents

    if (oper == "+") {
        result_ = left_val + right_val;
    } else if (oper == "-") {
        result_ = left_val - right_val;
    } else if (oper == "*") {
        result_ = left_val * right_val;
    } else if (oper == "/") {
        result_ = left_val / right_val;
    } else if (oper == "%") {
        result_ = left_val % right_val;
    } else if (oper == "^") {
        result_ = Pow(left_val, right_val);
    } else if (oper == ">") {
        result_ = left_val > right_val;
    } else if (oper == "<") {
        result_ = left_val < right_val;
    } else if (oper == ">=") {
        result_ = left_val >= right_val;
    } else if (oper == "<=") {
        result_ = left_val <= right_val;
    } else if (oper == "==") {
        result_ = left_val == right_val;
    } else if (oper == "!=") {
        result_ = left_val != right_val;
    } else {
        result_ = NullType{}; // TODO: error
    }
}

Value Evaluator::GetResult() const {
    return result_;
}

void Evaluator::Visit(Program& program) {
    for (const auto& stmt : program.GetStatements()) {
        stmt->Accept(*this);
    }
}

void Evaluator::Visit(IntegerLiteral& node) {
    result_ = node.value;
}

void Evaluator::Visit(BooleanLiteral& node) {
    result_ = node.value;
}

void Evaluator::Visit(NullTypeLiteral& node) {
    result_ = std::monostate{};
}

void Evaluator::Visit(FloatLiteral& node) {
    result_ = node.value;
}

void Evaluator::Visit(StringLiteral& node) {
    result_ = node.value;
}

void Evaluator::Visit(ExpressionStatement& node) {
    node.expr->Accept(*this);
}

void Evaluator::Visit(PrefixExpression& node) {
    Value right = Eval(*node.right);
    EvalPrefixExpression(node.oper, right);
}

void Evaluator::Visit(InfixExpression& node) {
    Value left = Eval(*node.left);
    Value right = Eval(*node.right);
    EvalInfixExpression(node.oper, left, right);
}

} // namespace ItmoScript
