#include "Evaluator.hpp"
#include "utils.hpp"

namespace ItmoScript {

Evaluator::Evaluator() {
    // RegisterUnaryOper<Int>("+", [this](const Value& right) { result_ = right.GetValue<Int>(); });
    // RegisterUnaryOper<Float>("+", [this](const Value& right) { result_ = right.GetValue<Float>(); });

    // RegisterUnaryOper<Int>("-", [this](const Value& right) { result_ = -right.GetValue<Int>(); });
    // RegisterUnaryOper<Float>("-", [this](const Value& right) { result_ = -right.GetValue<Float>(); });

    // RegisterBinaryOper<Int, Int>("+", [](const Value& right, const Value& left) { 
    //     return left.GetValue<Int>() + right.GetValue<Int>();
    // });

    // RegisterBinaryOper<Int, Int>("*", [](const Value& right, const Value& left) { 
    //     return left.GetValue<Int>() * right.GetValue<Int>();
    // });

    RegisterBinaryOper<Int, Int>("%", [](const Value& left, const Value& right) { 
        return left.GetValue<Int>() % right.GetValue<Int>();
    });

    RegisterBinaryOper<Int, Int>("^", [](const Value& left, const Value& right) { 
        return Utils::Pow(left.GetValue<Int>(), right.GetValue<Int>());
    });

    RegisterBinaryOper<Int, Int>("+", [](const Value& left, const Value& right) { 
        return left.GetValue<Int>() + right.GetValue<Int>();
    });

    RegisterBinaryOper<Int, Int>("-", [](const Value& left, const Value& right) { 
        return left.GetValue<Int>() - right.GetValue<Int>();
    });

    RegisterBinaryOper<Int, Int>("*", [](const Value& left, const Value& right) {
        return left.GetValue<Int>() * right.GetValue<Int>();
    });

    RegisterBinaryOper<Int, Int>("/", [](const Value& left, const Value& right) {
        return left.GetValue<Int>() / right.GetValue<Int>();
    });

    RegisterBinaryOper<Float, Float>("+", [](const Value& left, const Value& right) { 
        return left.GetValue<Float>() + right.GetValue<Float>();
    });

    RegisterBinaryOper<Float, Float>("-", [](const Value& left, const Value& right) { 
        return left.GetValue<Float>() - right.GetValue<Float>();
    });

    RegisterBinaryOper<Float, Float>("*", [](const Value& left, const Value& right) {
        return left.GetValue<Float>() * right.GetValue<Float>();
    });

    RegisterBinaryOper<Float, Float>("/", [](const Value& left, const Value& right) {
        return left.GetValue<Float>() / right.GetValue<Float>();
    });
}

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

std::optional<Value> Evaluator::HandleBinaryOper(const std::string& oper, const Value& left, const Value& right) {
    if (auto handler = FindExactHandler(oper, left, right)) {
        return std::invoke(*handler, left, right);
    }

    if (auto common = types_.FindCommonType(left, right)) {
        auto lhs = types_.TryConvert(left, *common);
        auto rhs = types_.TryConvert(right, *common);

        if (lhs && rhs) {
            if (auto handler = FindExactHandler(oper, *lhs, *rhs)) {
                return std::invoke(*handler, *lhs, *rhs);
            }
        }
    }

    return std::nullopt;
}

std::optional<Evaluator::BinaryHandler> // TODO: change left and right to type_index
Evaluator::FindExactHandler(const std::string& oper, const Value& left, const Value& right) {
    auto key = std::make_pair(left.GetTypeIndex(), right.GetTypeIndex());
    if (binary_ops_.contains(oper) && binary_ops_[oper].contains(key)) {
        return binary_ops_[oper][key];
    }

    return std::nullopt;
}

void Evaluator::AddUnaryOperatorForAllTypes(const std::string& oper, UnaryHandler handler) {
    RegisterUnaryOper<NullType>(oper, handler);
    RegisterUnaryOper<Int>(oper, handler);
    RegisterUnaryOper<Float>(oper, handler);
    RegisterUnaryOper<String>(oper, handler);
    RegisterUnaryOper<Bool>(oper, handler);
    RegisterUnaryOper<Function>(oper, handler);
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
}

void Evaluator::EvalInfixExpression(const std::string& oper, const Value& left, const Value& right) {
    if (left.IsInt() && right.IsInt()) {
        EvalIntInfixExpression(oper, left, right);
    } else if (left.IsBool() && right.IsBool()) {
        EvalBoolInfixExpression(oper, left, right);
    } else {
        result_ = NullType{}; // TODO: error
    }
}

void Evaluator::EvalIntInfixExpression(const std::string& oper, const Value& left, const Value& right) {
    Int left_val = left.GetValue<Int>();
    Int right_val = right.GetValue<Int>();

    // TODO: map for operators

    if (oper == "+") {
        result_ = left_val + right_val;
    } else if (oper == "-") {
        result_ = left_val - right_val;
    } else if (oper == "*") {
        result_ = left_val * right_val;
    } else if (oper == "/") {
        result_ = left_val / right_val; // TODO: check right_val == 0
    } else if (oper == "%") {
        result_ = left_val % right_val; // TODO: check right_val == 0
    } else if (oper == "^") {
        result_ = Utils::Pow(left_val, right_val); // TODO: negative exponents
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

void Evaluator::EvalBoolInfixExpression(const std::string& oper, const Value& left, const Value& right) {
    Bool left_val = left.GetValue<Bool>();
    Bool right_val = right.GetValue<Bool>();

    if (oper == "==") {
        result_ = (left_val == right_val);
    } else if (oper == "!=") {
        result_ = (left_val != right_val);
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
    
    if (auto new_value = HandleBinaryOper(node.oper, left, right)) {
        result_ = *new_value;
    } else {
        result_ = NullType{}; // TODO: error
    }
}

} // namespace ItmoScript
