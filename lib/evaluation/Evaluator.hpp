#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <typeindex>

#include "utils.hpp"

#include "ast/Visitor.hpp"
#include "ast/AST.hpp"

#include "evaluation/Value.hpp"
#include "evaluation/TypeSystem.hpp"
#include "evaluation/OperatorRegistry.hpp"
#include "evaluation/Environment.hpp"
#include "evaluation/FunctionObject.hpp"
#include "evaluation/CallFrame.hpp"

#include "exceptions/ZeroDivisionError.hpp"
#include "exceptions/SequenceMultiplicationError.hpp"

namespace itmoscript {

class Evaluator : public Visitor {
public:
    Evaluator();

    void Interpret(Program& root);
    const Value& GetLastEvaluatedValue() const;

    void ClearCallStack();

    void Visit(Program&) override;
    void Visit(ExpressionStatement&) override;
    void Visit(PrefixExpression&) override;
    void Visit(InfixExpression&) override;

    void Visit(IntegerLiteral&) override;
    void Visit(BooleanLiteral&) override;
    void Visit(NullTypeLiteral&) override;
    void Visit(FloatLiteral&) override;
    void Visit(StringLiteral&) override;
    void Visit(FunctionLiteral&) override;
    void Visit(Identifier&) override;

    void Visit(IfExpression&) override;
    void Visit(BlockStatement&) override;
    void Visit(AssignStatement&) override;
    void Visit(CallExpression&) override;
    void Visit(ReturnStatement&) override;

    void Visit(WhileStatement&) override;

    void Visit(ForStatement&) override {}
    void Visit(BreakStatement&) override {}
    void Visit(ContinueStatement&) override {}

private:
    Token current_token_;

    TypeSystem type_system_;
    OperatorRegistry operator_registry_;

    Environment global_env_;
    CallStack call_stack_;

    Value last_evaluated_value_;

    std::optional<Value> HandleUnaryOper(const std::string& oper, const Value& right);
    std::optional<Value> HandleBinaryOper(const std::string& oper, const Value& left, const Value& right);

    const Value& ResolveIdentifier(const Identifier& ident);
    void AssignIdentifier(const Identifier& ident, Value value);

    const Value& CallFunction(std::string name, const Function& func, std::vector<Value>& args);
    void EvalFunctionBody(const Function& func);
    std::string GetFunctionName(const std::optional<std::string>& name);

    const Value& Eval(Node& node);

    template<NumericValueType T>
    void RegisterCommonAriphmeticOps();

    template<NumericValueType T>
    void RegisterStringMultiplication();
    
    void RegisterTypeConversions();
    void RegisterAriphmeticOps();
    void RegisterUnaryOps();
    void RegisterComparisonOps();
    void RegisterStringOps();
    void RegisterLogicalOps();

    template<typename ErrorType, typename ...Args>
    void ThrowRuntimeError(Args&&... args) const noexcept(false);

    template<typename ErrorType, typename ...Args>
    void ThrowRuntimeError(Token token, Args&&... args) const noexcept(false);
};

template<NumericValueType T>
void Evaluator::RegisterCommonAriphmeticOps() {
    operator_registry_.RegisterBinaryOper<T, T>("+", 
        [](const Value& left, const Value& right) { return left.Get<T>() + right.Get<T>(); });

    operator_registry_.RegisterBinaryOper<T, T>("-", 
        [](const Value& left, const Value& right) { return left.Get<T>() - right.Get<T>(); });

    operator_registry_.RegisterBinaryOper<T, T>("*", 
        [](const Value& left, const Value& right) { return left.Get<T>() * right.Get<T>(); });

    operator_registry_.RegisterBinaryOper<T, T>("/", [this](const Value& left, const Value& right) { 
        if (right.Get<T>() == 0) {
            ThrowRuntimeError<lang_exceptions::ZeroDivisionError>();
        }
        
        return left.Get<T>() / right.Get<T>();
    });

    operator_registry_.RegisterUnaryOper<T>("+", [](const Value& right) { return right; });
    operator_registry_.RegisterUnaryOper<T>("-", [](const Value& right) { return -right.Get<T>(); });
}

template<NumericValueType T>
void Evaluator::RegisterStringMultiplication() {
    operator_registry_.RegisterCommutativeOperator<String, T>("*", [this](const Value& left, const Value& right) -> Value {
        String str = left.IsOfType<String>() ? left.Get<String>() : right.Get<String>();
        T number = left.IsOfType<String>() ? right.Get<T>() : left.Get<T>();

        std::optional<std::string> result = utils::MultiplyStr(str, number);
        if (!result) {
            ThrowRuntimeError<lang_exceptions::SequenceMultiplicationError>(
                std::format(
                    "cannot multiply string {} by negative value {}",
                    str,
                    number
                )
            );
        }

        return result.value();
    });
}

template<typename ErrorType, typename ...Args>
void Evaluator::ThrowRuntimeError(Token token, Args&&... args) const {
    throw ErrorType{token, call_stack_, std::forward<Args>(args)...};
}

template<typename ErrorType, typename ...Args>
void Evaluator::ThrowRuntimeError(Args&&... args) const {
    throw ErrorType{current_token_, call_stack_, std::forward<Args>(args)...};
}

} // namespace itmoscript
