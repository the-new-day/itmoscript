#include <format>
#include <cmath>

#include "Evaluator.hpp"
#include "utils.hpp"

namespace itmoscript {

Evaluator::Evaluator() {
    RegisterTypeConversions();
    RegisterAriphmeticOps();
    RegisterUnaryOps();
    RegisterComparisonOps();
    RegisterStringOps();
    RegisterLogicalOps();

    // operator_registry_.RegisterCommutativeOperatorForAllTypes<Bool>("and", [] (const Value& left, const Value& right) -> Value {
    //     return left.IsTruphy() && right.IsTruphy();
    // });
}

void Evaluator::Interpret(Program& root) {
    root.Accept(*this);
}

std::optional<Value> Evaluator::HandleUnaryOper(const std::string& oper, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, right.type())) {
        return std::invoke(*handler, right);
    }

    return std::nullopt;
}

std::optional<Value> Evaluator::HandleBinaryOper(const std::string& oper, const Value& left, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, left.type(), right.type())) {
        return std::invoke(*handler, left, right);
    }

    if (auto common = types_.FindCommonType(left.type(), right.type())) {
        auto lhs = types_.TryConvert(left, *common);
        auto rhs = types_.TryConvert(right, *common);

        if (lhs && rhs) {
            if (auto handler = operator_registry_.FindExactHandler(oper, *common, *common)) {
                return std::invoke(*handler, *lhs, *rhs);
            }
        }
    }

    return std::nullopt;
}

const Value& Evaluator::Eval(Node& node) {
    node.Accept(*this);
    return last_evaluated_value_;
}

void Evaluator::RegisterTypeConversions() {
    types_.RegisterConversion<Int, Float>([](Int value) { return static_cast<Float>(value); });
    types_.RegisterConversion<Float, Int>([](Float value) { return static_cast<Int>(value); });
}

void Evaluator::RegisterAriphmeticOps() {
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
}

void Evaluator::RegisterUnaryOps() {
    operator_registry_.RegisterUnaryOperatorForAllTypes("!", [](const Value& right) { return !right.IsTruphy(); });
    operator_registry_.RegisterUnaryOperatorForAllTypes("not", [](const Value& right) { return !right.IsTruphy(); });
}

void Evaluator::RegisterComparisonOps() {
    operator_registry_.RegisterAllComparisonOps<Int>();
    operator_registry_.RegisterAllComparisonOps<Float>();
    operator_registry_.RegisterAllComparisonOps<String>();

    operator_registry_.RegisterCommutativeOperatorForAllTypes<Bool>("==", [](const Value& left, const Value& right) {
        return left.IsTruphy() == right.IsTruphy();
    });

    operator_registry_.RegisterCommutativeOperatorForAllTypes<Bool>("!=", [](const Value& left, const Value& right) {
        return left.IsTruphy() != right.IsTruphy();
    });

    operator_registry_.RegisterCommutativeOperatorForAllTypes<NullType>("==", [](const Value& left, const Value& right) {
        return left.IsOfType<NullType>() && right.IsOfType<NullType>();
    });

    operator_registry_.RegisterCommutativeOperatorForAllTypes<NullType>("!=", [](const Value& left, const Value& right) {
        return left.type() != right.type();
    });

    operator_registry_.RegisterBinaryOper<String, String>("+", [](const Value& left, const Value& right) {
        return left.Get<String>() + right.Get<String>();
    });
}

void Evaluator::RegisterStringOps() {
    RegisterStringMultiplication<Int>();
    RegisterStringMultiplication<Float>();

    operator_registry_.RegisterBinaryOper<String, String>("-", [](const Value& left, const Value& right) -> Value {
        String str = left.Get<String>();
        String suffix = right.Get<String>();

        if (str.ends_with(suffix))
            return str.substr(0, str.size() - suffix.size());
        
        return str;
    });
}

void Evaluator::RegisterLogicalOps() {
    operator_registry_.RegisterCommutativeOperatorForAllPairsOfTypes("and", [](const Value& left, const Value& right) {
        return left.IsTruphy() && right.IsTruphy();
    });

    operator_registry_.RegisterCommutativeOperatorForAllPairsOfTypes("or", [](const Value& left, const Value& right) {
        return left.IsTruphy() || right.IsTruphy();
    });
}

const Value& Evaluator::GetLastEvaluatedValue() const {
    return last_evaluated_value_;
}

void Evaluator::Visit(Program& program) {
    for (const auto& stmt : program.GetStatements()) {
        current_token_ = program.token;
        stmt->Accept(*this);
    }
}

void Evaluator::Visit(IntegerLiteral& node) {
    current_token_ = node.token;
    last_evaluated_value_ = node.value;
}

void Evaluator::Visit(BooleanLiteral& node) {
    current_token_ = node.token;
    last_evaluated_value_ = node.value;
}

void Evaluator::Visit(NullTypeLiteral& node) {
    current_token_ = node.token;
    last_evaluated_value_ = std::monostate{};
}

void Evaluator::Visit(FloatLiteral& node) {
    current_token_ = node.token;
    last_evaluated_value_ = node.value;
}

void Evaluator::Visit(StringLiteral& node) {
    current_token_ = node.token;
    last_evaluated_value_ = node.value;
}

void Evaluator::Visit(Identifier& ident) {
    if (!env_.Has(ident.name)) {
        throw lang_exceptions::NameError{ident.token};
    }
    
    last_evaluated_value_ = env_.Get(ident.name); // TODO: reference for heavy types, copy for primitives
}

void Evaluator::Visit(AssignStatement& stmt) {
    env_.Set(stmt.ident->name, Eval(*stmt.expr));
}

void Evaluator::Visit(FunctionLiteral& func) {
    auto parameters = std::make_shared<std::vector<Identifier>>();
    parameters->reserve(func.parameters.size());

    for (std::unique_ptr<Identifier>& param : func.parameters) {
        parameters->push_back(std::move(*std::move(param)));
    }

    auto body = std::make_shared<BlockStatement>(std::move(*std::move(func.body)));
    auto env = std::make_shared<Environment>(env_);

    last_evaluated_value_ = std::make_shared<FunctionObject>(parameters, body, env);
}

void Evaluator::Visit(IfExpression& expr) {
    for (const auto& alternative : expr.alternatives) {
        if (alternative.condition == nullptr) { // final else branch, guaranteed to be last
            Eval(*alternative.consequence);
            return;
        }

        Value condition = Eval(*alternative.condition);
        if (condition.IsTruphy()) {
            Eval(*alternative.consequence);
            return;
        }
    }

    last_evaluated_value_ = NullType{};
}

void Evaluator::Visit(BlockStatement& block) {
    for (const auto& stmt : block.GetStatements()) {
        Eval(*stmt);
    }
}

void Evaluator::Visit(ExpressionStatement& node) {
    current_token_ = node.token;
    node.expr->Accept(*this);
}

void Evaluator::Visit(PrefixExpression& node) {
    current_token_ = node.token;
    Value right = Eval(*node.right);
    
    if (auto new_value = HandleUnaryOper(node.oper, right)) {
        last_evaluated_value_ = *new_value;
    } else {
        throw lang_exceptions::OperatorTypeError{current_token_, node.oper, right.type()};
    }
}

void Evaluator::Visit(InfixExpression& node) {
    current_token_ = node.token;
    Value left = Eval(*node.left);
    Value right = Eval(*node.right);

    if (auto new_value = HandleBinaryOper(node.oper, left, right)) {
        last_evaluated_value_ = *new_value;
    } else {
        throw lang_exceptions::OperatorTypeError{current_token_, node.oper, left.type(), right.type()};
    }
}

} // namespace itmoscript
