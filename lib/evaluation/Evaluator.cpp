#include "Evaluator.hpp"
#include "utils.hpp"

#include "exceptions/OperatorTypeError.hpp"
#include "exceptions/UndefinedNameError.hpp"
#include "exceptions/ParametersCountError.hpp"
#include "exceptions/DuplicateParameterError.hpp"
#include "exceptions/UnexpectedReturnError.hpp"
#include "exceptions/IndexOperandTypeError.hpp"
#include "exceptions/IndexTypeError.hpp"
#include "exceptions/IndexOutOfRangeError.hpp"
#include "exceptions/NegativeIndexError.hpp"

#include <format>
#include <cmath>
#include <set>
#include <algorithm>
#include <limits>

namespace itmoscript {

Evaluator::Evaluator() {
    RegisterTypeConversions();
    RegisterAriphmeticOps();
    RegisterUnaryOps();
    RegisterComparisonOps();
    RegisterStringOps();
    RegisterLogicalOps();

    env_stack_.emplace(std::make_shared<Environment>(nullptr));
}

void Evaluator::Interpret(ast::Program& root) {
    call_stack_.clear();
    last_exec_result_ = Eval(root);
}

std::optional<Value> Evaluator::HandleUnaryOper(TokenType oper, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, right.type())) {
        return std::invoke(*handler, right);
    }

    return std::nullopt;
}

std::optional<Value> Evaluator::HandleBinaryOper(TokenType oper, const Value& left, const Value& right) {
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
    return *env_stack_.top();
}

void Evaluator::PushEnv() {
    env_stack_.emplace(std::make_shared<Environment>(env_stack_.top()));
}

void Evaluator::PopEnv() {
    env_stack_.pop();
}

const Evaluator::ExecResult& Evaluator::Eval(ast::Node& node) {
    current_token_ = node.token;
    node.Accept(*this);
    return last_exec_result_;
}

const Value& Evaluator::GetLastEvaluatedValue() const {
    return last_exec_result_.value;
}

const Value& Evaluator::ResolveIdentifier(const ast::Identifier& ident) {
    if (!env().Has(ident.name)) {
        ThrowRuntimeError<lang_exceptions::UndefinedNameError>(ident.token);
    }

    return env().Get(ident.name);
}

void Evaluator::AssignIdentifier(const ast::Identifier& ident, Value value) {
    env().Set(ident.name, std::move(value));
}

void Evaluator::Visit(ast::Program& program) {
    for (const auto& stmt : program.GetStatements()) {
        current_token_ = program.token;
        stmt->Accept(*this);
    }
}

void Evaluator::Visit(ast::IntegerLiteral& node) {
    last_exec_result_.value = node.value;
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::BooleanLiteral& node) {
    last_exec_result_.value = node.value;
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::NullTypeLiteral& node) {
    last_exec_result_.value = NullType{};
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::FloatLiteral& node) {
    last_exec_result_.value = node.value;
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::StringLiteral& node) {
    last_exec_result_.value = CreateString(node.value);
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::Identifier& node) {
    last_exec_result_.value = ResolveIdentifier(node);
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::AssignStatement& stmt) {
    Eval(*stmt.expr);
    AssignIdentifier(*stmt.ident, last_exec_result_.value);
}

void Evaluator::Visit(ast::OperatorAssignStatement& stmt) {
    ExecResult right_res = Eval(*stmt.expr);
    Value ident_value = ResolveIdentifier(*stmt.ident);

    if (auto new_value = HandleBinaryOper(kCompoundAssignOperators.at(stmt.oper), ident_value, right_res.value)) {
        last_exec_result_.value = *new_value;
        AssignIdentifier(*stmt.ident, last_exec_result_.value);
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(
            kTokenTypeNames.at(stmt.oper),
            ident_value.type(),
            right_res.value.type()
        );
    }
}

void Evaluator::Visit(ast::CallExpression& expr) {
    std::vector<Value> args;
    args.reserve(expr.arguments.size());
    
    for (auto& arg : expr.arguments) {
        args.push_back(Eval(*arg).value);
    }

    Function func = Eval(*expr.function).value.Get<Function>();

    last_exec_result_.value = CallFunction(GetFunctionName(expr.function_name), func, args);
    last_exec_result_.control = ControlFlowState::kNormal;
}

Value Evaluator::CallFunction(std::string name, const Function& func, std::vector<Value>& args) {
    if (args.size() != func.parameters().size()) {
        ThrowRuntimeError<lang_exceptions::ParametersCountError>(func.parameters().size(), args.size());
    }

    PushEnv();
    
    call_stack_.push_back(CallFrame{.function_name = std::move(name), .entry_token = current_token_});

    for (size_t i = 0; i < func.parameters().size(); ++i) {
        const std::string& name = func.parameters().at(i)->name;
        env().SetInLocal(name, std::move(args[i]));
    }

    ExecResult res = Eval(*func.body());
    PopEnv();
    call_stack_.pop_back();

    if (res.control == ControlFlowState::kReturn) {
        return res.value;
    } else {
        return NullType{};
    }
}

void Evaluator::Visit(ast::ReturnStatement& stmt) {
    if (call_stack_.empty()) {
        ThrowRuntimeError<lang_exceptions::UnexpectedReturnError>();
    }

    if (stmt.expr != nullptr) {
        last_exec_result_ = Eval(*stmt.expr);
    } else {
        last_exec_result_.value = NullType{};
    }

    last_exec_result_.control = ControlFlowState::kReturn;
}

void Evaluator::Visit(ast::WhileStatement& stmt) {
    bool prev_loop = inside_loop_;
    inside_loop_ = true;

    last_exec_result_.value = NullType{};
    last_exec_result_.control = ControlFlowState::kNormal;

    while (Eval(*stmt.condition).value.IsTruphy()) {
        Eval(*stmt.body);
        
        if (last_exec_result_.control == ControlFlowState::kContinue) {
            last_exec_result_.control = ControlFlowState::kNormal;
            continue;
        } else if (last_exec_result_.control == ControlFlowState::kBreak) {
            last_exec_result_.control = ControlFlowState::kNormal;
            break;
        } else if (last_exec_result_.control == ControlFlowState::kReturn) {
            break;
        }
    }

    inside_loop_ = prev_loop;
}

void Evaluator::Visit(ast::BreakStatement& stmt) {
    if (!inside_loop_) {
        // TODO: error
    }
    last_exec_result_.control = ControlFlowState::kBreak;
}

void Evaluator::Visit(ast::ContinueStatement& stmt) {
    if (!inside_loop_) {
        // TODO: error
    }
    last_exec_result_.control = ControlFlowState::kContinue;
}

void Evaluator::Visit(ast::ListLiteral& list_literal) {
    std::vector<Value> elements;
    elements.reserve(list_literal.elements.size());
    
    for (auto& expr : list_literal.elements) {
        elements.push_back(Eval(*expr).value);
    }

    last_exec_result_.value = std::make_shared<ListObject>(std::move(elements));
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::FunctionLiteral& func) {
    std::set<std::string> seen_params;

    for (const std::shared_ptr<ast::Identifier>& param : func.parameters) {
        if (seen_params.contains(param->name)) {
            ThrowRuntimeError<lang_exceptions::DuplicateParameterError>(param->name);
        }

        seen_params.insert(param->name);
    }

    auto parameters = std::make_shared<std::vector<std::shared_ptr<ast::Identifier>>>(func.parameters);
    last_exec_result_.value = Function(std::move(parameters), func.body);
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::IfExpression& expr) {
    for (const auto& alternative : expr.alternatives) {
        if (alternative.condition == nullptr) { // else-branch, guaranteed to be last
            Eval(*alternative.consequence);
            return;
        }

        Eval(*alternative.condition);

        if (last_exec_result_.value.IsTruphy()) {
            Eval(*alternative.consequence);
            return;
        }

        if (last_exec_result_.control != ControlFlowState::kNormal) {
            return;
        }
    }

    last_exec_result_.control = ControlFlowState::kNormal;
    last_exec_result_.value = NullType{};
}

void Evaluator::Visit(ast::BlockStatement& block) {
    PushEnv();

    for (const auto& stmt : block.GetStatements()) {
        Eval(*stmt);
        if (last_exec_result_.control != ControlFlowState::kNormal) {
            break;
        }
    }

    PopEnv();
}

void Evaluator::Visit(ast::ExpressionStatement& node) {
    current_token_ = node.token;
    node.expr->Accept(*this);
}

void Evaluator::Visit(ast::PrefixExpression& node) {
    ExecResult right_res = Eval(*node.right);
    
    if (auto new_value = HandleUnaryOper(node.oper, right_res.value)) {
        last_exec_result_.value = *new_value;
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(
            kTokenTypeNames.at(node.oper), 
            right_res.value.type()
        );
    }
}

void Evaluator::Visit(ast::InfixExpression& node) {
    ExecResult left_res = Eval(*node.left);
    ExecResult right_res = Eval(*node.right);
    
    if (auto new_value = HandleBinaryOper(node.oper, left_res.value, right_res.value)) {
        last_exec_result_.value = *new_value;
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(
            kTokenTypeNames.at(node.oper),
            left_res.value.type(), 
            right_res.value.type()
        );
    }
}

void Evaluator::Visit(ast::IndexOperatorExpression& expr) {
    ExecResult operand = Eval(*expr.operand);
    ValueType op_type = operand.value.type();
        
    last_exec_result_.control = ControlFlowState::kNormal;

    if (op_type != ValueType::kString && op_type != ValueType::kList) {
        ThrowRuntimeError<lang_exceptions::IndexOperandTypeError>(op_type);
    }

    ExecResult index = Eval(*expr.index);

    if (index.value.type() != ValueType::kInt) {
        ThrowRuntimeError<lang_exceptions::IndexTypeError>(index.value.type());
    } else if (index.value.Get<Int>() < 0) {
        ThrowRuntimeError<lang_exceptions::NegativeIndexError>(index.value.Get<Int>());
    }

    size_t pos = index.value.Get<Int>();

    if (!expr.is_slice) {
        if (op_type == ValueType::kString) {
            const String& str = operand.value.Get<String>();
            if (pos >= str->size()) {
                ThrowRuntimeError<lang_exceptions::IndexOutOfRangeError>(pos, str->size());
            }
            last_exec_result_.value = CreateString(std::string{str->at(pos)});
        } else if (op_type == ValueType::kList) {
            const List& list = operand.value.Get<List>();
            if (pos >= list->size()) {
                ThrowRuntimeError<lang_exceptions::IndexOutOfRangeError>(pos, list->size());
            }
            last_exec_result_.value = list->data().at(pos);
        }
    }

    if (expr.is_slice) {
        ExecResult second_index = {.value = std::numeric_limits<int64_t>::max()};
        if (expr.second_index != nullptr) {
            second_index = Eval(*expr.index);
        }

        if (second_index.value.type() != ValueType::kInt) {
            ThrowRuntimeError<lang_exceptions::IndexTypeError>(second_index.value.type());
        } else if (index.value.Get<Int>() < 0) {
            ThrowRuntimeError<lang_exceptions::NegativeIndexError>(second_index.value.Get<Int>());
        }

        size_t end = second_index.value.Get<Int>();

        if (op_type == ValueType::kString) {
            const String& str = operand.value.Get<String>();
            if (str->empty() || pos < end) {
                last_exec_result_.value = CreateString(std::string{});
                return;
            }

            pos = std::min(pos, str->size() - 1);
            end = std::min(end, str->size() - 1);
            
            last_exec_result_.value = CreateString(std::string(
                str->begin() + pos,
                str->begin() + end
            ));
            return;
        } else if (op_type == ValueType::kList) {
            const List& list = operand.value.Get<List>();
            last_exec_result_.value = CreateList(list->GetSlice(pos, end));
        }
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

    operator_registry_.RegisterBinaryOper<Int, Int>(TokenType::kPercent, [](const Value& left, const Value& right) {
        return left.Get<Int>() % right.Get<Int>();
    });

    operator_registry_.RegisterBinaryOper<Int, Int>(TokenType::kPow, [](const Value& left, const Value& right) -> Value {
        if (right.Get<Int>() < 0) {
            return utils::FastPowNeg(left.Get<Int>(), right.Get<Int>());
        }
        return utils::FastPow(left.Get<Int>(), right.Get<Int>());
    });

    operator_registry_.RegisterBinaryOper<Float, Float>(TokenType::kPow, [](const Value& left, const Value& right) {
        return std::pow(left.Get<Float>(), right.Get<Float>());
    });
}

void Evaluator::RegisterUnaryOps() {
    operator_registry_.RegisterUnaryOperatorForAllTypes(
        TokenType::kBang, 
        [](const Value& right) { return !right.IsTruphy(); }
    );

    operator_registry_.RegisterUnaryOperatorForAllTypes(
        TokenType::kNot, 
        [](const Value& right) { return !right.IsTruphy(); }
    );
}

void Evaluator::RegisterComparisonOps() {
    operator_registry_.RegisterAllComparisonOps<Int>();
    operator_registry_.RegisterAllComparisonOps<Float>();
    operator_registry_.RegisterAllComparisonOpsForHeavyType<String>();

    operator_registry_.RegisterCommutativeOperatorForAllTypes<Bool>(
        TokenType::kEqual, 
        [](const Value& left, const Value& right) {
            return left.IsTruphy() == right.IsTruphy();
        }
    );

    operator_registry_.RegisterCommutativeOperatorForAllTypes<Bool>(
        TokenType::kNotEqual, 
        [](const Value& left, const Value& right) {
            return left.IsTruphy() != right.IsTruphy();
        }
    );

    operator_registry_.RegisterCommutativeOperatorForAllTypes<NullType>(
        TokenType::kEqual, 
        [](const Value& left, const Value& right) {
            return left.IsOfType<NullType>() && right.IsOfType<NullType>();
        }
    );

    operator_registry_.RegisterCommutativeOperatorForAllTypes<NullType>(
        TokenType::kNotEqual, 
        [](const Value& left, const Value& right) {
            return left.type() != right.type();
        }
    );

    operator_registry_.RegisterBinaryOper<Function, Function>(
        TokenType::kEqual, 
        [](const Value& left, const Value& right) {
            return left.Get<Function>() == right.Get<Function>();
        }
    );

    operator_registry_.RegisterBinaryOper<Function, Function>(
        TokenType::kNotEqual, 
        [](const Value& left, const Value& right) {
            return left.Get<Function>() != right.Get<Function>();
        }
    );

    operator_registry_.RegisterBinaryOper<List, List>(
        TokenType::kEqual, 
        [](const Value& left, const Value& right) {
            return *left.Get<List>() == *right.Get<List>();
        }
    );

    operator_registry_.RegisterBinaryOper<List, List>(
        TokenType::kNotEqual, 
        [](const Value& left, const Value& right) {
            return *left.Get<List>() != *right.Get<List>();
        }
    );
}

void Evaluator::RegisterStringOps() {
    RegisterStringMultiplication<Int>();
    RegisterStringMultiplication<Float>();

    operator_registry_.RegisterBinaryOper<String, String>(
        TokenType::kMinus, 
        [this](const Value& left, const Value& right) -> Value {
            const String& str = left.Get<String>();
            const String& suffix = right.Get<String>();

            if (str->ends_with(*suffix))
                return CreateString(str->substr(0, str->size() - suffix->size()));
            
            return CreateString(*str);
        }
    );
    
    operator_registry_.RegisterBinaryOper<String, String>(
        TokenType::kPlus, 
        [this](const Value& left, const Value& right) {
            return CreateString(*left.Get<String>() + *right.Get<String>());
        }
    );
}

void Evaluator::RegisterLogicalOps() {
    operator_registry_.RegisterCommutativeOperatorForAllPairsOfTypes(
        TokenType::kAnd, 
        [](const Value& left, const Value& right) {
            return left.IsTruphy() && right.IsTruphy();
        }
    );

    operator_registry_.RegisterCommutativeOperatorForAllPairsOfTypes(
        TokenType::kOr, 
        [](const Value& left, const Value& right) {
            return left.IsTruphy() || right.IsTruphy();
        }
    );
}

} // namespace itmoscript
