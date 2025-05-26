#include "Evaluator.hpp"
#include "utils.hpp"

#include "exceptions/OperatorTypeError.hpp"
#include "exceptions/UndefinedNameError.hpp"
#include "exceptions/ParametersCountError.hpp"
#include "exceptions/DuplicateParameterError.hpp"
#include "exceptions/UnexpectedReturnError.hpp"

#include <format>
#include <cmath>
#include <unordered_set>

namespace itmoscript {

Evaluator::Evaluator() {
    RegisterTypeConversions();
    RegisterAriphmeticOps();
    RegisterUnaryOps();
    RegisterComparisonOps();
    RegisterStringOps();
    RegisterLogicalOps();
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

    if (auto common = type_system_.FindCommonType(left.type(), right.type())) {
        auto lhs = type_system_.TryConvert(left, *common);
        auto rhs = type_system_.TryConvert(right, *common);

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

const Value& Evaluator::GetLastEvaluatedValue() const {
    return last_evaluated_value_;
}

void Evaluator::ClearCallStack() {
    call_stack_.clear();   
}

const Value& Evaluator::ResolveIdentifier(const Identifier& ident) {
    if (!call_stack_.empty()) {
        const auto& local_env = call_stack_.back().local_env;
        if (local_env.Has(ident.name)) {
            return local_env.Get(ident.name);
        }
    }

    if (!global_env_.Has(ident.name)) {
        ThrowRuntimeError<lang_exceptions::UndefinedNameError>(ident.token);
    }

    return global_env_.Get(ident.name);
}

void Evaluator::AssignIdentifier(const Identifier& ident, Value value) {
    if (call_stack_.empty()) {
        global_env_.Set(ident.name, std::move(value));
        return;
    }
    
    auto& local_env = call_stack_.back().local_env;

    if (local_env.Has(ident.name) || !global_env_.Has(ident.name)) {
        local_env.Set(ident.name, std::move(value));
    } else if (global_env_.Has(ident.name)) {
        global_env_.Set(ident.name, std::move(value));
    }
}

const Value& Evaluator::CallFunction(const Function& func, std::vector<Value>& args) {
    if (args.size() != func->parameters->size()) {
        ThrowRuntimeError<lang_exceptions::ParametersCountError>(func->parameters->size(), args.size());
    }

    CallFrame frame;
    frame.entry_token = current_token_;
    frame.function_name = "<Anonymous function>"; // TODO: real func name if exists

    for (size_t i = 0; i < func->parameters->size(); ++i) {
        const std::string& name = func->parameters->at(i).name;
        frame.local_env.Set(name, std::move(args[i]));
    }

    call_stack_.push_back(std::move(frame));

    EvalFunctionBody(func);
    auto& frame_result = call_stack_.back();

    if (frame_result.return_value.has_value()) {
        last_evaluated_value_ = std::move(*frame_result.return_value);
    }
    
    call_stack_.pop_back();
    return last_evaluated_value_;
}

void Evaluator::EvalFunctionBody(const Function& func) {
    Eval(*func->body);
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
    last_evaluated_value_ = ResolveIdentifier(ident);
}

void Evaluator::Visit(AssignStatement& stmt) {
    AssignIdentifier(*stmt.ident, Eval(*stmt.expr));
}

void Evaluator::Visit(CallExpression& expr) {
    std::vector<Value> args;
    args.reserve(expr.arguments.size());
    
    for (auto& arg : expr.arguments) {
        args.push_back(Eval(*arg));
    }

    const Function& func = Eval(*expr.function).Get<Function>();
    CallFunction(func, args);
}

void Evaluator::Visit(ReturnStatement& stmt) {
    if (call_stack_.empty()) {
        ThrowRuntimeError<lang_exceptions::UnexpectedReturnError>();
    }

    auto& current_frame = call_stack_.back();

    if (stmt.expr != nullptr)
        current_frame.return_value = Eval(*stmt.expr);
}

void Evaluator::Visit(FunctionLiteral& func) {
    auto parameters = std::make_shared<std::vector<Identifier>>();
    parameters->reserve(func.parameters.size());

    std::unordered_set<std::string> seen_params;

    auto body = std::make_shared<BlockStatement>(std::move(*std::move(func.body)));

    for (std::unique_ptr<Identifier>& param : func.parameters) {
        if (seen_params.contains(param->name)) {
            ThrowRuntimeError<lang_exceptions::DuplicateParameterError>(param->name);
        }

        seen_params.insert(param->name);
        parameters->push_back(std::move(*std::move(param)));
    }

    last_evaluated_value_ = std::make_shared<FunctionObject>(parameters, body);
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
        if (!call_stack_.empty() && call_stack_.back().return_value.has_value()) {
            return;
        }
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
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(node.oper, right.type());
    }
}

void Evaluator::Visit(InfixExpression& node) {
    current_token_ = node.token;
    Value left = Eval(*node.left);
    Value right = Eval(*node.right);

    if (auto new_value = HandleBinaryOper(node.oper, left, right)) {
        last_evaluated_value_ = *new_value;
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(node.oper, left.type(), right.type());
    }
}

void Evaluator::RegisterTypeConversions() {
    type_system_.RegisterConversion<Int, Float>([](Int value) { return static_cast<Float>(value); });
    type_system_.RegisterConversion<Float, Int>([](Float value) { return static_cast<Int>(value); });
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

} // namespace itmoscript
