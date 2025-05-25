#include <format>
#include <cmath>

#include "Evaluator.hpp"
#include "utils.hpp"

namespace itmoscript {

Evaluator::Evaluator() {
    types_.RegisterConversion<Int, Float>([](Int value) { return static_cast<Float>(value); });
    types_.RegisterConversion<Float, Int>([](Float value) { return static_cast<Int>(value); });

    RegisterCommonAriphmeticOps<Int>();
    RegisterCommonAriphmeticOps<Float>();

    operator_registry_.RegisterBinaryOper<Int, Int>("%", [](const Value& left, const Value& right) {
        return left.Get<Int>() % right.Get<Int>();
    });

    operator_registry_.RegisterBinaryOper<Int, Int>("^", [](const Value& left, const Value& right) -> Value {
        if (right.Get<Int>() < 0) {
            return utils::FastPowNeg(left.Get<Int>(), right.Get<Int>());
        }
        return utils::FastPow(left.Get<Int>(), right.Get<Int>());
    });

    operator_registry_.RegisterBinaryOper<Float, Float>("^", [](const Value& left, const Value& right) {
        return std::pow(left.Get<Float>(), right.Get<Float>());
    });

    operator_registry_.RegisterUnaryOperatorForAllTypes("!", [](const Value& right) { return !right.IsTruphy(); });

    operator_registry_.RegisterAllComparisonOps<Int>();
    operator_registry_.RegisterAllComparisonOps<Float>();
    operator_registry_.RegisterAllComparisonOps<String>();
    
    operator_registry_.RegisterBinaryOper<Bool, Bool>("==", [](const Value& left, const Value& right) {
        return left.Get<Bool>() == right.Get<Bool>();
    });
    
    operator_registry_.RegisterBinaryOper<Bool, Bool>("!=", [](const Value& left, const Value& right) {
        return left.Get<Bool>() != right.Get<Bool>();
    });

    operator_registry_.RegisterCommutativeOperatorForAllTypes<NullType>("==", [](const Value& left, const Value& right) {
        return left.IsOfType<NullType>() && right.IsOfType<NullType>();
    });

    operator_registry_.RegisterCommutativeOperatorForAllTypes<NullType>("!=", [](const Value& left, const Value& right) {
        return left.GetType() != right.GetType();
    });

    operator_registry_.RegisterBinaryOper<String, String>("+", [](const Value& left, const Value& right) {
        return left.Get<String>() + right.Get<String>();
    });

    RegisterStringMultiplication<Int>();
    RegisterStringMultiplication<Float>();

    operator_registry_.RegisterBinaryOper<String, String>("-", [](const Value& left, const Value& right) -> Value {
        String str = left.Get<String>();
        String suffix = right.Get<String>();

        if (str.ends_with(suffix))
            return str.substr(0, str.size() - suffix.size());
        
        return str;
    });

    // operator_registry_.RegisterCommutativeOperatorForAllTypes<Bool>("and", [] (const Value& left, const Value& right) -> Value {
    //     return left.IsTruphy() && right.IsTruphy();
    // });

    // TODO: add comparison with bool (5 == true)
}

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

std::optional<Value> Evaluator::HandleUnaryOper(const std::string& oper, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, right.GetType())) {
        return std::invoke(*handler, right);
    }

    return std::nullopt;
}

std::optional<Value> Evaluator::HandleBinaryOper(const std::string& oper, const Value& left, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, left.GetType(), right.GetType())) {
        return std::invoke(*handler, left, right);
    }

    if (auto common = types_.FindCommonType(left, right)) {
        auto lhs = types_.TryConvert(left, *common);
        auto rhs = types_.TryConvert(right, *common);

        if (lhs && rhs) {
            if (auto handler = operator_registry_.FindExactHandler(oper, lhs->GetType(), rhs->GetType())) {
                return std::invoke(*handler, *lhs, *rhs);
            }
        }
    }

    return std::nullopt;
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

void Evaluator::Visit(Identifier& ident) {
    // TODO:
    result_ = NullType{};
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
        throw lang_exceptions::OperatorTypeError{current_token_, node.oper, right.GetType()};
    }
}

void Evaluator::Visit(InfixExpression& node) {
    current_token_ = node.token;
    Value left = Eval(*node.left);
    Value right = Eval(*node.right);

    if (auto new_value = HandleBinaryOper(node.oper, left, right)) {
        result_ = *new_value;
    } else {
        throw lang_exceptions::OperatorTypeError{current_token_, node.oper, left.GetType(), right.GetType()};
    }
}

} // namespace itmoscript
