#include "Evaluator.hpp"

namespace ItmoScript {

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

Value Evaluator::Eval(Node& node) {
    node.Accept(*this);
    return result_;
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

void Evaluator::Visit(ExpressionStatement& node) {
    node.expr->Accept(*this);
}

} // namespace ItmoScript
