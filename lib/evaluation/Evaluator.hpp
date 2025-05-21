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
#include "utils.hpp"

namespace ItmoScript {

struct EvaluationError {
    std::string message;
    Token token;
};
    
class Evaluator : public Visitor {
private:
    using BinaryHandler = std::function<Value(const Value&, const Value&)>;
    using UnaryHandler = std::function<Value(const Value&)>;

public:
    Evaluator();

    void Interpret(Program& root);
    Value GetResult() const;

    template<SupportedValueType Right>
    void RegisterUnaryOper(const std::string& oper, UnaryHandler handler);

    template<SupportedValueType Left, SupportedValueType Right>
    void RegisterBinaryOper(const std::string& oper, BinaryHandler handler);

    const std::vector<EvaluationError>& GetErrors() const { return errors_; }

    void Visit(Program&) override;
    void Visit(ExpressionStatement&) override;
    void Visit(PrefixExpression&) override;
    void Visit(InfixExpression&) override;

    void Visit(IntegerLiteral&) override;
    void Visit(BooleanLiteral&) override;
    void Visit(NullTypeLiteral&) override;
    void Visit(FloatLiteral&) override;
    void Visit(StringLiteral&) override;

    void Visit(Identifier&) override {}
    void Visit(AssignStatement&) override {}
    void Visit(ReturnStatement&) override {}
    void Visit(BlockStatement&) override {}
    void Visit(IfExpression&) override {}
    void Visit(FunctionLiteral&) override {}
    void Visit(CallExpression&) override {}
    void Visit(WhileStatement&) override {}
    void Visit(ForStatement&) override {}
    void Visit(BreakStatement&) override {}
    void Visit(ContinueStatement&) override {}

private:
    Value result_;

    std::unordered_map<
        std::string, 
        std::unordered_map<
            std::pair<std::type_index, std::type_index>,
            BinaryHandler,
            Utils::TypePairHash
        >
    > binary_ops_;

    std::unordered_map<
        std::string, 
        std::unordered_map<
            std::type_index,
            UnaryHandler
        >
    > unary_ops_;

    TypeSystem types_;
    std::vector<EvaluationError> errors_;
    Token current_token_;

    std::optional<Value> HandleUnaryOper(const std::string& oper, const Value& right);
    std::optional<Value> HandleBinaryOper(const std::string& oper, const Value& left, const Value& right);
    
    std::optional<BinaryHandler> FindExactHandler(const std::string& oper, const Value& left, const Value& right);
    std::optional<UnaryHandler> FindExactHandler(const std::string& oper, const Value& right);

    void AddUnaryOperatorForAllTypes(const std::string& oper, UnaryHandler handler);

    template<SupportedValueType T, SupportedValueType U>
    void RegisterCommutativeOperator(const std::string& oper, BinaryHandler handler);

    template<SupportedValueType T>
    void AddCommutativeOperatorForAllTypes(const std::string& oper, BinaryHandler handler);

    Value Eval(Node& node);

    template<SupportedValueType T>
    void RegisterCommonAriphmeticOps();

    template<SupportedValueType T>
    void RegisterComparisonOps();

    template<NumericValueType T>
    void RegisterStringMultiplication();

    void AddError(const std::string& message);
};

template<SupportedValueType Right>
void Evaluator::RegisterUnaryOper(const std::string& oper, UnaryHandler handler) {
    unary_ops_[oper][typeid(Right)] = handler;
}

template<SupportedValueType Left, SupportedValueType Right>
void Evaluator::RegisterBinaryOper(const std::string& oper, BinaryHandler handler) {
    binary_ops_[oper][{typeid(Left), typeid(Right)}] = handler;
}

template<SupportedValueType T, SupportedValueType U>
void Evaluator::RegisterCommutativeOperator(const std::string& oper, BinaryHandler handler) {
    binary_ops_[oper][{typeid(T), typeid(U)}] = handler;
    binary_ops_[oper][{typeid(U), typeid(T)}] = handler;
}

template<SupportedValueType T>
void Evaluator::AddCommutativeOperatorForAllTypes(const std::string& oper, BinaryHandler handler) {
    RegisterCommutativeOperator<T, NullType>(oper, handler);
    RegisterCommutativeOperator<T, Int>(oper, handler);
    RegisterCommutativeOperator<T, Float>(oper, handler);
    RegisterCommutativeOperator<T, String>(oper, handler);
    RegisterCommutativeOperator<T, Bool>(oper, handler);
    RegisterCommutativeOperator<T, Function>(oper, handler);
}

template<SupportedValueType T>
void Evaluator::RegisterCommonAriphmeticOps() {
    RegisterBinaryOper<T, T>("+", [](const Value& left, const Value& right) { return left.GetValue<T>() + right.GetValue<T>(); });
    RegisterBinaryOper<T, T>("-", [](const Value& left, const Value& right) { return left.GetValue<T>() - right.GetValue<T>(); });
    RegisterBinaryOper<T, T>("*", [](const Value& left, const Value& right) { return left.GetValue<T>() * right.GetValue<T>(); });
    RegisterBinaryOper<T, T>("/", [this](const Value& left, const Value& right) { 
        if (right.GetValue<T>() == 0) {
            AddError("division by zero");
            return Value{NullType{}}; // TODO: proper error
        }
        
        return Value{left.GetValue<T>() / right.GetValue<T>()};
    });

    RegisterUnaryOper<T>("+", [](const Value& right) { return right; });
    RegisterUnaryOper<T>("-", [](const Value& right) { return -right.GetValue<T>(); });
}

template<SupportedValueType T>
void Evaluator::RegisterComparisonOps() {
    const auto cmp = [](auto op) {
        return [op](const Value& left, const Value& right) {
            return op(left.GetValue<T>(), right.GetValue<T>());
        };
    };
    
    RegisterBinaryOper<T, T>("==", cmp(std::equal_to{}));
    RegisterBinaryOper<T, T>("!=", cmp(std::not_equal_to{}));
    RegisterBinaryOper<T, T>("<", cmp(std::less{}));
    RegisterBinaryOper<T, T>("<=", cmp(std::less_equal{}));
    RegisterBinaryOper<T, T>(">", cmp(std::greater{}));
    RegisterBinaryOper<T, T>(">=", cmp(std::greater_equal{}));
}

template<NumericValueType T>
void Evaluator::RegisterStringMultiplication() {
    RegisterCommutativeOperator<String, T>("*", [this](const Value& left, const Value& right) -> Value {
        String str = left.IsOfType<String>() ? left.GetValue<String>() : right.GetValue<String>();
        T number = left.IsOfType<String>() ? right.GetValue<T>() : left.GetValue<T>();

        std::optional<std::string> result = Utils::MultiplyStr(str, number);
        if (!result) {
            AddError(std::format(
                "cannot multiply string {} by negative value {}",
                str,
                number
            ));
            return NullType{}; // TODO: proper error
        }

        return result.value();
    });
}

} // namespace ItmoScript
