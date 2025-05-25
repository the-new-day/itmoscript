#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <typeindex>

#include "ast/Visitor.hpp"
#include "ast/AST.hpp"
#include "evaluation/Value.hpp"
#include "evaluation/TypeSystem.hpp"
#include "evaluation/OperatorRegistry.hpp"
#include "evaluation/Environment.hpp"
#include "utils.hpp"

#include "exceptions/OperatorTypeError.hpp"
#include "exceptions/ZeroDivisionError.hpp"
#include "exceptions/SequenceMultiplicationError.hpp"
#include "exceptions/NameError.hpp"

namespace itmoscript {
    
class Evaluator : public Visitor {
public:
    Evaluator();

    void Interpret(Program& root);
    Value GetLastEvaluatedValue() const;

    void Visit(Program&) override;
    void Visit(ExpressionStatement&) override;
    void Visit(PrefixExpression&) override;
    void Visit(InfixExpression&) override;

    void Visit(IntegerLiteral&) override;
    void Visit(BooleanLiteral&) override;
    void Visit(NullTypeLiteral&) override;
    void Visit(FloatLiteral&) override;
    void Visit(StringLiteral&) override;

    // TODO: implement
    void Visit(Identifier&) override;
    void Visit(FunctionLiteral&) override;

    void Visit(IfExpression&) override;
    void Visit(BlockStatement&) override;
    void Visit(AssignStatement&) override;

    void Visit(ReturnStatement&) override {}
    void Visit(CallExpression&) override {}
    void Visit(WhileStatement&) override {}
    void Visit(ForStatement&) override {}
    void Visit(BreakStatement&) override {}
    void Visit(ContinueStatement&) override {}

private:
    Value last_evaluated_value_;
    Token current_token_;

    TypeSystem types_;
    OperatorRegistry operator_registry_;
    Environment env_;

    std::optional<Value> HandleUnaryOper(const std::string& oper, const Value& right);
    std::optional<Value> HandleBinaryOper(const std::string& oper, const Value& left, const Value& right);

    Value Eval(Node& node);

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
            throw lang_exceptions::ZeroDivisionError{current_token_};
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
            throw lang_exceptions::SequenceMultiplicationError{
                current_token_, 
                std::format(
                    "cannot multiply string {} by negative value {}",
                    str,
                    number
                )
            };
        }

        return result.value();
    });
}

} // namespace itmoscript
