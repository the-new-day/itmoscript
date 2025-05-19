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
    
class Evaluator : public Visitor {
private:
    using BinaryHandler = std::function<Value(const Value&, const Value&)>;
    using UnaryHandler = std::function<Value(const Value&)>;

public:
    Evaluator();

    void Interpret(Program& root);
    Value GetResult() const;

    template<typename Right>
    void RegisterUnaryOper(const std::string& oper, UnaryHandler handler);

    template<typename Left, typename Right>
    void RegisterBinaryOper(const std::string& oper, BinaryHandler handler);

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

    std::optional<Value> HandleBinaryOper(const std::string& oper, const Value& left, const Value& right);
    std::optional<BinaryHandler> FindExactHandler(const std::string& oper, const Value& left, const Value& right);

    void AddUnaryOperatorForAllTypes(const std::string& oper, UnaryHandler handler);

    Value Eval(Node& node);

    void EvalPrefixExpression(const std::string& oper, const Value& right);
    void EvalBangOperatorExpression(const Value& right);
    void EvalUnaryMinusOperatorExpression(const Value& right);
    void EvalUnaryPlusOperatorExpression(const Value& right);

    void EvalInfixExpression(const std::string& oper, const Value& left, const Value& right);
    void EvalIntInfixExpression(const std::string& oper, const Value& left, const Value& right);
    void EvalBoolInfixExpression(const std::string& oper, const Value& left, const Value& right);

    template<typename T>
    std::optional<T> PerformOperation(const std::string& oper, const T& left, const T& right);
};

template<typename Right>
void Evaluator::RegisterUnaryOper(const std::string& oper, UnaryHandler handler) {
    unary_ops_[oper][typeid(Right)] = handler;
}

template<typename Left, typename Right>
void Evaluator::RegisterBinaryOper(const std::string& oper, BinaryHandler handler) {
    binary_ops_[oper][{typeid(Left), typeid(Right)}] = handler;
}

template<typename T>
std::optional<T> Evaluator::PerformOperation(const std::string& oper, const T& left, const T& right) {
    if (oper == "+") {
        return left + right;
    } else if (oper == "*") {
        return left * right;
    } else if (oper == "-") {
        return left - right;
    } else if (oper == "/") {
        return left / right;
    } else if (oper == "%") {
        return left % right;
    } else if (oper == "^") {
        return Utils::Pow(left, right); // TODO: negative exponents
    } else if (oper == ">") {
        return left > right;
    } else if (oper == ">") {
        return left > right;
    } else if (oper == "<") {
        return left <= right;
    } else if (oper == ">=") {
        return left >= right;
    } else if (oper == "<=") {
        return left <= right;
    } else if (oper == "==") {
        return left == right;
    } else if (oper == "!=") {
        return left != right;
    } else {
        return std::nullopt;
    }
}

} // namespace ItmoScript
