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

    env_stack_.emplace_back(std::make_shared<Environment>(nullptr));
}

void Evaluator::Interpret(Program& root) {
    call_stack_.clear();
    Eval(root);
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

Environment& Evaluator::env() {
    return env_stack_.back();
}

void Evaluator::PushEnv() {
    env_stack_.emplace_back(std::make_shared<Environment>(env()));
}

void Evaluator::PopEnv() {
    env_stack_.pop_back();
}

Evaluator::ExecResult Evaluator::Eval(Node& node) {
    result_stack_.emplace_back();
    node.Accept(*this);
    ExecResult res = std::move(result_stack_.back());
    result_stack_.pop_back();
    return res;
}

const Value& Evaluator::GetLastEvaluatedValue() const {
    return result_stack_.back().value;
}

const Value& Evaluator::ResolveIdentifier(const Identifier& ident) {
    if (!env().Has(ident.name)) {
        ThrowRuntimeError<lang_exceptions::UndefinedNameError>(ident.token);
    }

    return env().Get(ident.name);
}

void Evaluator::AssignIdentifier(const Identifier& ident, Value value) {
    env().Set(ident.name, std::move(value));
}

void Evaluator::Visit(Program& program) {
    for (const auto& stmt : program.GetStatements()) {
        current_token_ = program.token;
        stmt->Accept(*this);
    }
}

void Evaluator::Visit(IntegerLiteral& node) {
    current_token_ = node.token;
    auto& top = result_stack_.back();
    top.value = node.value;
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(BooleanLiteral& node) {
    current_token_ = node.token;
    auto& top = result_stack_.back();
    top.value = node.value;
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(NullTypeLiteral& node) {
    current_token_ = node.token;
    auto& top = result_stack_.back();
    top.value = NullType{};
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(FloatLiteral& node) {
    current_token_ = node.token;
    auto& top = result_stack_.back();
    top.value = node.value;
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(StringLiteral& node) {
    current_token_ = node.token;
    auto& top = result_stack_.back();
    top.value = node.value;
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(Identifier& node) {
    current_token_ = node.token;
    auto& top = result_stack_.back();
    top.value = ResolveIdentifier(node);
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(AssignStatement& stmt) {
    AssignIdentifier(*stmt.ident, Eval(*stmt.expr).value);
}

void Evaluator::Visit(CallExpression& expr) {
    std::vector<Value> args;
    args.reserve(expr.arguments.size());
    
    for (auto& arg : expr.arguments) {
        args.push_back(Eval(*arg).value);
    }

    Function func = Eval(*expr.function).value.Get<Function>();

    auto& top = result_stack_.back();
    top.value = CallFunction(GetFunctionName(expr.function_name), func, args);
    top.control = ControlFlowState::kNormal;
}

Value Evaluator::CallFunction(std::string name, const Function& func, std::vector<Value>& args) {
    if (args.size() != func->parameters->size()) {
        ThrowRuntimeError<lang_exceptions::ParametersCountError>(func->parameters->size(), args.size());
    }

    PushEnv();
    
    call_stack_.push_back(CallFrame{.function_name = std::move(name), .entry_token = current_token_});

    for (size_t i = 0; i < func->parameters->size(); ++i) {
        const std::string& name = func->parameters->at(i).name;
        env().Set(name, std::move(args[i]));
    }

    ExecResult res = Eval(*func->body);
    PopEnv();
    call_stack_.pop_back();

    if (res.control == ControlFlowState::kReturn) {
        return res.value;
    } else {
        return NullType{};
    }
}

void Evaluator::Visit(ReturnStatement& stmt) {
    if (call_stack_.empty()) {
        ThrowRuntimeError<lang_exceptions::UnexpectedReturnError>();
    }

    ExecResult val;
    if (stmt.expr != nullptr) {
        val = Eval(*stmt.expr);
    } else {
        val.value = NullType{};
    }

    val.control = ControlFlowState::kReturn;
    result_stack_.back() = std::move(val);
}

void Evaluator::Visit(WhileStatement& stmt) {
    bool prev_loop = inside_loop_;
    inside_loop_ = true;

    auto& top = result_stack_.back();
    top.value = NullType{};
    top.control = ControlFlowState::kNormal;

    while (Eval(*stmt.condition).value.IsTruphy()) {
        ExecResult res = Eval(*stmt.body);
        
        if (res.control == ControlFlowState::kContinue) {
            continue;
        } else if (res.control == ControlFlowState::kBreak) {
            top.control = ControlFlowState::kNormal;
            break;
        } else if (res.control == ControlFlowState::kReturn) {
            top = std::move(res);
            break;
        }
    }

    inside_loop_ = prev_loop;
}

void Evaluator::Visit(BreakStatement& stmt) {
    if (!inside_loop_) {
        // TODO: error
    }
    result_stack_.back().control = ControlFlowState::kBreak;
}

void Evaluator::Visit(ContinueStatement& stmt) {
    if (!inside_loop_) {
        // TODO: error
    }
    result_stack_.back().control = ControlFlowState::kContinue;
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

    result_stack_.back().value = std::make_shared<FunctionObject>(parameters, body);
    result_stack_.back().control = ControlFlowState::kNormal;
}

void Evaluator::Visit(IfExpression& expr) {
    for (const auto& alternative : expr.alternatives) {
        if (alternative.condition == nullptr) { // else-branch, guaranteed to be last
            result_stack_.back() = Eval(*alternative.consequence);
            return;
        }

        ExecResult cond_res = Eval(*alternative.condition);

        if (cond_res.value.IsTruphy()) {
            result_stack_.back() = Eval(*alternative.consequence);
            return;
        }

        if (cond_res.control != ControlFlowState::kNormal) {
            result_stack_.back() = std::move(cond_res);
            return;
        }
    }

    auto& top = result_stack_.back();
    top.value = NullType{};
    top.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(BlockStatement& block) {
    PushEnv();

    for (const auto& stmt : block.GetStatements()) {
        ExecResult res = Eval(*stmt);
        if (res.control != ControlFlowState::kNormal) {
            result_stack_.back() = std::move(res);
            PopEnv();
            return;
        }
    }

    auto& top = result_stack_.back();
    top.value = NullType{};
    top.control = ControlFlowState::kNormal;
    PopEnv();
}

void Evaluator::Visit(ExpressionStatement& node) {
    current_token_ = node.token;
    node.expr->Accept(*this);
}

void Evaluator::Visit(PrefixExpression& node) {
    ExecResult right = Eval(*node.right);

    if (right.control != ControlFlowState::kNormal) {
        result_stack_.back() = std::move(right);
        return;
    }

    if (auto new_value = HandleUnaryOper(node.oper, right.value)) {
        auto& top = result_stack_.back();
        top.value = std::move(*new_value);
        top.control = ControlFlowState::kNormal;
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(node.oper, right.value.type());
    }
}

void Evaluator::Visit(InfixExpression& node) {
    ExecResult left = Eval(*node.left);

    if (left.control != ControlFlowState::kNormal) {
        result_stack_.back() = std::move(left);
        return;
    }

    ExecResult right = Eval(*node.right);

    if (right.control != ControlFlowState::kNormal) {
        result_stack_.back() = std::move(right);
        return;
    }

    if (auto new_value = HandleBinaryOper(node.oper, left.value, right.value)) {
        auto& top = result_stack_.back();
        top.value = std::move(*new_value);
        top.control = ControlFlowState::kNormal;
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(node.oper, left.value.type(), right.value.type());
    }
}

std::string Evaluator::GetFunctionName(const std::optional<std::string>& name) {
    return name.has_value() ? *name : "<anonymous function>";
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
