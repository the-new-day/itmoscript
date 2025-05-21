#include "Evaluator.hpp"
#include "utils.hpp"

#include <format>
#include <cmath>

namespace ItmoScript {

Evaluator::Evaluator() {
    RegisterCommonAriphmeticOps<Int>();
    RegisterCommonAriphmeticOps<Float>();

    RegisterBinaryOper<Int, Int>("%", [](const Value& left, const Value& right) {
        return left.Get<Int>() % right.Get<Int>();
    });

    RegisterBinaryOper<Int, Int>("^", [](const Value& left, const Value& right) -> Value {
        if (right.Get<Int>() < 0) {
            return Utils::FastPowNeg(left.Get<Int>(), right.Get<Int>());
        }
        return Utils::FastPow(left.Get<Int>(), right.Get<Int>());
    });

    RegisterBinaryOper<Float, Float>("^", [](const Value& left, const Value& right) {
        return std::pow(left.Get<Float>(), right.Get<Float>());
    });

    AddUnaryOperatorForAllTypes("!", [](const Value& right) { return !right.IsTruphy(); });

    RegisterComparisonOps<Int>();
    RegisterComparisonOps<Float>();
    RegisterComparisonOps<String>();
    
    RegisterBinaryOper<Bool, Bool>("==", [](const Value& left, const Value& right) {
        return left.Get<Bool>() == right.Get<Bool>();
    });
    
    RegisterBinaryOper<Bool, Bool>("!=", [](const Value& left, const Value& right) {
        return left.Get<Bool>() != right.Get<Bool>();
    });

    AddCommutativeOperatorForAllTypes<NullType>("==", [](const Value& left, const Value& right) {
        return left.IsNullType() && right.IsNullType();
    });

    AddCommutativeOperatorForAllTypes<NullType>("!=", [](const Value& left, const Value& right) {
        return left.GetType() != right.GetType();
    });

    RegisterBinaryOper<String, String>("+", [](const Value& left, const Value& right) {
        return left.Get<String>() + right.Get<String>();
    });

    RegisterStringMultiplication<Int>();
    RegisterStringMultiplication<Float>();

    RegisterBinaryOper<String, String>("-", [](const Value& left, const Value& right) -> Value {
        String str = left.Get<String>();
        String suffix = right.Get<String>();

        if (str.ends_with(suffix))
            return str.substr(0, str.size() - suffix.size());
        
        return str;
    });
}

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

std::optional<Value> Evaluator::HandleUnaryOper(const std::string& oper, const Value& right) {
    if (auto handler = FindExactHandler(oper, right)) {
        return std::invoke(*handler, right);
    }

    return std::nullopt;
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

std::optional<Evaluator::UnaryHandler>
Evaluator::FindExactHandler(const std::string& oper, const Value& right) {
    if (unary_ops_.contains(oper) && unary_ops_[oper].contains(right.GetTypeIndex())) {
        return unary_ops_[oper][right.GetTypeIndex()];
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

Value Evaluator::GetResult() const {
    return result_;
}

void Evaluator::Visit(Program& program) {
    for (const auto& stmt : program.GetStatements()) {
        current_token_ = program.token;
        stmt->Accept(*this);
    }
}

void Evaluator::Visit(IntegerLiteral& node) {
    current_token_ = node.token;
    result_ = node.value;
}

void Evaluator::Visit(BooleanLiteral& node) {
    current_token_ = node.token;
    result_ = node.value;
}

void Evaluator::Visit(NullTypeLiteral& node) {
    current_token_ = node.token;
    result_ = std::monostate{};
}

void Evaluator::Visit(FloatLiteral& node) {
    current_token_ = node.token;
    result_ = node.value;
}

void Evaluator::Visit(StringLiteral& node) {
    current_token_ = node.token;
    result_ = node.value;
}

void Evaluator::Visit(ExpressionStatement& node) {
    current_token_ = node.token;
    node.expr->Accept(*this);
}

void Evaluator::Visit(PrefixExpression& node) {
    current_token_ = node.token;
    Value right = Eval(*node.right);
    
    if (auto new_value = HandleUnaryOper(node.oper, right)) {
        result_ = *new_value;
    } else {
        AddError(std::format(
            "unsupported operand type for '{}': {}",
            node.oper,
            kValueTypeNames.at(right.GetType())
        ));

        result_ = NullType{}; // TODO: proper error
    }
}

void Evaluator::Visit(InfixExpression& node) {
    current_token_ = node.token;
    Value left = Eval(*node.left);
    Value right = Eval(*node.right);
    
    if (auto new_value = HandleBinaryOper(node.oper, left, right)) {
        result_ = *new_value;
    } else {
        AddError(std::format(
            "unsupported operand types for '{}': {} and {}",
            node.oper, 
            kValueTypeNames.at(left.GetType()), 
            kValueTypeNames.at(right.GetType())
        ));

        result_ = NullType{}; // TODO: proper error
    }
}

void Evaluator::AddError(const std::string& message) {
    errors_.push_back({.message = message, .token = current_token_});
}

} // namespace ItmoScript
