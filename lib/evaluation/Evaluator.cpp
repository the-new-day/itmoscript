#include "Evaluator.hpp"

namespace ItmoScript {

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

Value Evaluator::Eval(Node& node) {
    node.Accept(*this);
    return result_;
}

void Evaluator::EvalPrefixExpression(std::string oper, const Value& right) {
    // TODO: make map with handlers (?)
    if (oper == "!") {
        EvalBangOperatorExpression(right);
    } else if (oper == "-") {
        EvalUnaryMinusOperatorExpression(right);
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

} // namespace ItmoScript
