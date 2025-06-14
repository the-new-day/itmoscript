#include "Evaluator.hpp"
#include "utils.hpp"

#include "exceptions/OperatorTypeError.hpp"
#include "exceptions/UndefinedNameError.hpp"
#include "exceptions/ParametersCountError.hpp"
#include "exceptions/DuplicateParameterError.hpp"
#include "exceptions/IndexOperandTypeError.hpp"
#include "exceptions/IndexTypeError.hpp"
#include "exceptions/IndexOutOfRangeError.hpp"
#include "exceptions/NegativeIndexError.hpp"
#include "exceptions/ControlFlowError.hpp"
#include "exceptions/UnsupportedTypeError.hpp"
#include "exceptions/StandardFunctionNoCallError.hpp"
#include "exceptions/UncallableObjectCallError.hpp"
#include "exceptions/StandardOverrideError.hpp"
#include "exceptions/ImmutableAssignmentError.hpp"

#include <format>
#include <cmath>
#include <set>
#include <algorithm>
#include <limits>

namespace itmoscript {

Evaluator::Evaluator() {
    RegisterTypeConversions();
    env_stack_.emplace_back(std::make_shared<Environment>(nullptr));
}

void Evaluator::Evaluate(ast::Program& root, std::istream& input, std::ostream& output) {
    input_ = &input;
    output_ = &output;
    call_stack_.clear();
    last_exec_result_ = Eval(root);
}

void Evaluator::EnableStandardOperators() {
    RegisterAriphmeticOps();
    RegisterUnaryOps();
    RegisterComparisonOps();
    RegisterStringOps();
    RegisterLogicalOps();
    RegisterListOps();
}

void Evaluator::EnableStd() {
    std_lib_.LoadDefault();
}

std::optional<Value> Evaluator::HandleUnaryOper(TokenType oper, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, right.GetType())) {
        return std::invoke(*handler, right);
    }

    return std::nullopt;
}

std::optional<Value> Evaluator::HandleBinaryOper(TokenType oper, const Value& left, const Value& right) {
    if (auto handler = operator_registry_.FindExactHandler(oper, left.GetType(), right.GetType())) {
        return std::invoke(*handler, left, right);
    }

    if (auto common = type_convertion_system_.FindCommonType(left.GetType(), right.GetType())) {
        auto lhs = type_convertion_system_.TryConvert(left, *common);
        auto rhs = type_convertion_system_.TryConvert(right, *common);

        if (lhs && rhs) {
            if (auto handler = operator_registry_.FindExactHandler(oper, *common, *common)) {
                return std::invoke(*handler, *lhs, *rhs);
            }
        }
    }

    return std::nullopt;
}

Environment& Evaluator::env() {
    return *env_stack_.back();
}

void Evaluator::PushEnv() {
    env_stack_.emplace_back(std::make_shared<Environment>(env_stack_.back()));
}

void Evaluator::PopEnv() {
    env_stack_.pop_back();
}

void Evaluator::EnterFunctionScope() {
    env_stack_.emplace_back(std::make_shared<Environment>(env_stack_.front()));
}

void Evaluator::LeaveFunctionScope() {
    PopEnv();
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
        if (std_lib_.Has(ident.name)) {
            ThrowRuntimeError<lang_exceptions::StandardFunctionNoCallError>(ident.token);
        } else {
            ThrowRuntimeError<lang_exceptions::UndefinedNameError>(ident.token);
        }
    }

    return env().Get(ident.name);
}

void Evaluator::AssignIdentifier(const std::string& name, Value value) {
    if (env().Has(name) && env().Get(name).IsOfType<Function>()) {
        ThrowRuntimeError<lang_exceptions::ImmutableAssignmentError>(
            name, GetType<Function>()
        );
    }

    env().Set(name, std::move(value));
}


void Evaluator::AssignIdentifierWithCopy(const std::string& name, Value value) {
    if (!value.IsReferenceType() || !env().Has(name)) {
        AssignIdentifier(name, std::move(value));
        return;
    }

    Value env_val = env().Get(name);
    if (env_val.GetType() != value.GetType()) {
        AssignIdentifier(name, std::move(value));
        return;
    }

    if (value.IsOfType<List>()) {
        *env_val.Get<List>() = std::move(*value.GetCopy().Get<List>());
    } else if (value.IsOfType<String>()) {
        *env_val.Get<String>() = std::move(*value.GetCopy().Get<String>());
    } else if (value.IsOfType<Function>()) {
        AssignIdentifier(name, std::move(value));
    }
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
    if (std_lib_.Has(stmt.ident->name)) {
        ThrowRuntimeError<lang_exceptions::StandardOverrideError>(stmt.ident->name);
    }

    Eval(*stmt.expr);

    if (!env().Has(stmt.ident->name) || dynamic_cast<ast::Identifier*>(stmt.expr.get())) {
        AssignIdentifier(stmt.ident->name, last_exec_result_.value);
    } else {
        AssignIdentifierWithCopy(stmt.ident->name, last_exec_result_.value);
    }
}

void Evaluator::Visit(ast::OperatorAssignStatement& stmt) {
    if (std_lib_.Has(stmt.ident->name)) {
        ThrowRuntimeError<lang_exceptions::StandardOverrideError>(stmt.ident->name);
    }

    ExecResult right_res = Eval(*stmt.expr);
    Value ident_value = ResolveIdentifier(*stmt.ident);

    if (auto new_value = HandleBinaryOper(kCompoundAssignOperators.at(stmt.oper), ident_value, right_res.value)) {
        last_exec_result_.value = *new_value;
        AssignIdentifier(stmt.ident->name, last_exec_result_.value);
    } else {
        ThrowRuntimeError<lang_exceptions::OperatorTypeError>(
            kTokenTypeNames.at(stmt.oper),
            ident_value.GetType(),
            right_res.value.GetType()
        );
    }
}

void Evaluator::Visit(ast::CallExpression& expr) {
    std::vector<Value> args;
    args.reserve(expr.arguments.size());
    
    for (auto& arg : expr.arguments) {
        args.push_back(Eval(*arg).value);
    }

    if (expr.function_name && !env().Has(*expr.function_name)) {
        if (std_lib_.HasOutStreamHandlingFunc(*expr.function_name)) {
            CallOutStreamLibraryFunction(*expr.function_name, args);
            return;
        } else if (std_lib_.HasInStreamHandlingFunc(*expr.function_name)) {
            CallInStreamLibraryFunction(*expr.function_name, args);
            return;
        } else if (std_lib_.HasValueHandlingFunc(*expr.function_name)) {
            CallLibraryFunction(*expr.function_name, args);
            return;
        }
    }

    ExecResult func_result = Eval(*expr.function);
    if (func_result.value.GetType() != ValueType::kFunction) {
        ThrowRuntimeError<lang_exceptions::UncallableObjectCallError>(expr.function->String());
    }

    const Function& func = func_result.value.Get<Function>();

    last_exec_result_.value = CallFunction(GetFunctionName(expr.function_name), func, args);
    last_exec_result_.control = ControlFlowState::kNormal;
}

Value Evaluator::CallFunction(std::string name, const Function& func, std::vector<Value>& args) {
    if (args.size() != func.parameters().size()) {
        ThrowRuntimeError<lang_exceptions::ParametersCountError>(
            name,
            func.parameters().size(),
            args.size()
        );
    }

    EnterFunctionScope();
    
    call_stack_.push_back(CallFrame{.function_name = std::move(name), .entry_token = current_token_});

    for (size_t i = 0; i < func.parameters().size(); ++i) {
        const std::string& param_name = func.parameters().at(i)->name;
        env().SetInLocal(param_name, std::move(args[i]));
    }

    ExecResult res = Eval(*func.body());
    LeaveFunctionScope();
    call_stack_.pop_back();

    if (res.control == ControlFlowState::kReturn) {
        return res.value;
    } else {
        return NullType{};
    }
}

void Evaluator::CallLibraryFunction(const std::string& name, std::vector<Value>& args) {
    last_exec_result_.value = std_lib_.Call(name, args, current_token_, call_stack_);
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::CallOutStreamLibraryFunction(const std::string& name, std::vector<Value>& args) {
    last_exec_result_.value = std_lib_.CallOutStreamHandlingFunc(
        *output_, 
        name,
        args, 
        current_token_, 
        call_stack_
    );
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::CallInStreamLibraryFunction(const std::string& name, std::vector<Value>& args) {
    last_exec_result_.value = std_lib_.CallInStreamHandlingFunc(
        *input_, 
        name,
        args, 
        current_token_, 
        call_stack_
    );
    last_exec_result_.control = ControlFlowState::kNormal;
}

void Evaluator::Visit(ast::ReturnStatement& stmt) {
    if (call_stack_.empty()) {
        ThrowRuntimeError<lang_exceptions::ControlFlowError>("unexpected 'return' outside of a function");
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
        
        if (last_exec_result_.control == ControlFlowState::kBreak) {
            last_exec_result_.control = ControlFlowState::kNormal;
            break;
        } else if (last_exec_result_.control == ControlFlowState::kReturn) {
            break;
        }
    }

    inside_loop_ = prev_loop;
}

void Evaluator::Visit(ast::ForStatement& stmt) {
    bool prev_loop = inside_loop_;
    inside_loop_ = true;

    last_exec_result_.value = NullType{};
    last_exec_result_.control = ControlFlowState::kNormal;

    ExecResult range_res = Eval(*stmt.range);

    if (range_res.value.GetType() != ValueType::kList) {
        ThrowRuntimeError<lang_exceptions::UnsupportedTypeError>(
            "range in the for loop is not of type List"
        );
    }

    const List& range = range_res.value.Get<List>();

    for (const Value& value : range->data()) {
        PushEnv();
        env().SetInLocal(stmt.iter->name, value);
        Eval(*stmt.body);

        PopEnv();

        if (last_exec_result_.control == ControlFlowState::kBreak) {
            last_exec_result_.control = ControlFlowState::kNormal;
            break;
        } else if (last_exec_result_.control == ControlFlowState::kReturn) {
            break;
        }
    }
}

void Evaluator::Visit(ast::BreakStatement& stmt) {
    if (!inside_loop_) {
        ThrowRuntimeError<lang_exceptions::ControlFlowError>("unexpected 'break' outside of a loop");
    }
    last_exec_result_.control = ControlFlowState::kBreak;
}

void Evaluator::Visit(ast::ContinueStatement& stmt) {
    if (!inside_loop_) {
        ThrowRuntimeError<lang_exceptions::ControlFlowError>("unexpected 'continue' outside of a loop");
    }
    last_exec_result_.control = ControlFlowState::kContinue;
}

void Evaluator::Visit(ast::ListLiteral& list_literal) {
    std::vector<Value> elements;
    elements.reserve(list_literal.elements.size());
    
    for (auto& expr : list_literal.elements) {
        elements.push_back(Eval(*expr).value);
    }

    last_exec_result_.value = CreateList(std::move(elements));
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
            last_exec_result_ = Eval(*alternative.consequence);
            return;
        }

        Eval(*alternative.condition);

        if (last_exec_result_.value.IsTruphy()) {
            last_exec_result_ = Eval(*alternative.consequence);
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
    if (block.GetStatements().empty()) {
        last_exec_result_.value = NullType{};
        return;
    }

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
            right_res.value.GetType()
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
            left_res.value.GetType(), 
            right_res.value.GetType()
        );
    }
}

void Evaluator::Visit(ast::IndexOperatorExpression& expr) {
    if (expr.is_slice) {
        EvalSliceIndexExpression(expr);
        return;
    }

    ExecResult operand = Eval(*expr.operand);
    ValueType op_type = operand.value.GetType();
        
    last_exec_result_.control = ControlFlowState::kNormal;

    if (op_type != ValueType::kString && op_type != ValueType::kList) {
        ThrowRuntimeError<lang_exceptions::IndexOperandTypeError>(op_type);
    }

    ExecResult index = Eval(*expr.index);

    if (index.value.GetType() != ValueType::kInt) {
        ThrowRuntimeError<lang_exceptions::IndexTypeError>(index.value.GetType());
    }

    Int given_pos = index.value.Get<Int>();

    if (op_type == ValueType::kString) {
        const String& str = operand.value.Get<String>();
        size_t pos = given_pos >= 0 ? given_pos : str->size() + given_pos;

        if (pos >= str->size()) {
            ThrowRuntimeError<lang_exceptions::IndexOutOfRangeError>(given_pos, str->size());
        }

        last_exec_result_.value = CreateString(std::string{str->at(pos)});
    } else if (op_type == ValueType::kList) {
        const List& list = operand.value.Get<List>();
        size_t pos = given_pos >= 0 ? given_pos : list->size() + given_pos;
        
        if (pos >= list->size()) {
            ThrowRuntimeError<lang_exceptions::IndexOutOfRangeError>(given_pos, list->size());
        }

        last_exec_result_.value = list->At(pos);
    }
}

void Evaluator::EvalSliceIndexExpression(ast::IndexOperatorExpression& expr) {
    ExecResult operand = Eval(*expr.operand);
    ValueType op_type = operand.value.GetType();

    if (op_type != ValueType::kString && op_type != ValueType::kList) {
        ThrowRuntimeError<lang_exceptions::IndexOperandTypeError>(op_type);
    }

    size_t start = 0;
    size_t end = std::numeric_limits<size_t>::max();

    if (expr.index != nullptr) {
        ExecResult index_res = Eval(*expr.index);
        
        if (index_res.value.GetType() != ValueType::kInt) {
            ThrowRuntimeError<lang_exceptions::IndexTypeError>(index_res.value.GetType());
        } else if (index_res.value.Get<Int>() < 0) {
            ThrowRuntimeError<lang_exceptions::NegativeIndexError>(index_res.value.Get<Int>());
        }

        start = index_res.value.Get<Int>();
    }

    if (expr.second_index != nullptr) {
        ExecResult index_res = Eval(*expr.second_index);
        
        if (index_res.value.GetType() != ValueType::kInt) {
            ThrowRuntimeError<lang_exceptions::IndexTypeError>(index_res.value.GetType());
        } else if (index_res.value.Get<Int>() < 0) {
            ThrowRuntimeError<lang_exceptions::NegativeIndexError>(index_res.value.Get<Int>());
        }

        end = index_res.value.Get<Int>();
    }

    last_exec_result_.control = ControlFlowState::kNormal;

    if (op_type == ValueType::kList) {
        const List& list = operand.value.Get<List>();
        last_exec_result_.value = CreateList(list->GetSlice(start, end));
    } else if (op_type == ValueType::kString) {
        const String& str = operand.value.Get<String>();

        if (start > end || start >= str->size()) {
            last_exec_result_.value = CreateString(std::string{});
            return;
        }

        if (end >= str->size())
            end = str->size();

        last_exec_result_.value = CreateString(std::string(
            str->begin() + start,
            str->begin() + end
        ));
    }
}

std::string Evaluator::GetFunctionName(const std::optional<std::string>& name) {
    return name.has_value() ? *name : "<anonymous function>";
}

void Evaluator::RegisterTypeConversions() {
    type_convertion_system_.RegisterConversion<Int, Float>(
        [](Int value) { 
            return static_cast<Float>(value); 
        }
    );
    type_convertion_system_.RegisterConversion<Float, Int>(
        [](Float value) { 
            return static_cast<Int>(value); 
        }
    );
    type_convertion_system_.RegisterConversion<Bool, Int>(
        [](Bool value) { 
            return value ? 1 : 0;
        }
    );
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
    operator_registry_.RegisterAllComparisonOpsForRefType<String>();

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
            return left.GetType() != right.GetType();
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
    RegisterStringMultiplication<Bool>();

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

void Evaluator::RegisterListOps() {
    RegisterListMultiplication<Int>();
    RegisterListMultiplication<Float>();
    
    operator_registry_.RegisterBinaryOper<List, List>(
        TokenType::kPlus, 
        [this](const Value& left, const Value& right) {
            std::vector<Value> result = left.Get<List>()->data();
            const std::vector<Value>& right_vals = right.Get<List>()->data();
            result.insert(result.end(), right_vals.begin(), right_vals.end());

            return CreateList(result);
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
