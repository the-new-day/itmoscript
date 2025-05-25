#pragma once

#include <unordered_map>
#include <string>

#include "Value.hpp"
#include "TypeSystem.hpp"

namespace itmoscript {

class OperatorRegistry {
public:
    using BinaryHandler = std::function<Value(const Value&, const Value&)>;
    using UnaryHandler = std::function<Value(const Value&)>;

    template<CoreValueType Right>
    void RegisterUnaryOper(const std::string& oper, UnaryHandler handler);

    template<CoreValueType Left, CoreValueType Right>
    void RegisterBinaryOper(const std::string& oper, BinaryHandler handler);

    template<CoreValueType T, CoreValueType U>
    void RegisterCommutativeOperator(const std::string& oper, BinaryHandler handler);

    template<CoreValueType T>
    void RegisterCommutativeOperatorForAllTypes(const std::string& oper, BinaryHandler handler);

    void RegisterUnaryOperatorForAllTypes(const std::string& oper, UnaryHandler handler);

    template<CoreValueType T>
    void RegisterAllComparisonOps();

    std::optional<BinaryHandler> FindExactHandler(const std::string& oper, ValueType left, ValueType right);
    std::optional<UnaryHandler> FindExactHandler(const std::string& oper, ValueType type);

private:
    std::unordered_map<
        std::string, 
        std::unordered_map<
            std::pair<ValueType, ValueType>,
            BinaryHandler,
            ValueTypePairHash
        >
    > binary_ops_;

    std::unordered_map<
        std::string, 
        std::unordered_map<
            ValueType,
            UnaryHandler
        >
    > unary_ops_;
};

template<CoreValueType Right>
void OperatorRegistry::RegisterUnaryOper(const std::string& oper, UnaryHandler handler) {
    unary_ops_[oper][TypeSystem::GetValueType<Right>()] = handler;
}

template<CoreValueType Left, CoreValueType Right>
void OperatorRegistry::RegisterBinaryOper(const std::string& oper, BinaryHandler handler) {
    binary_ops_[oper][{TypeSystem::GetValueType<Left>(), TypeSystem::GetValueType<Right>()}] = handler;
}

template<CoreValueType T, CoreValueType U>
void OperatorRegistry::RegisterCommutativeOperator(const std::string& oper, BinaryHandler handler) {
    binary_ops_[oper][{TypeSystem::GetValueType<T>(), TypeSystem::GetValueType<U>()}] = handler;
    binary_ops_[oper][{TypeSystem::GetValueType<U>(), TypeSystem::GetValueType<T>()}] = handler;
}

template<CoreValueType T>
void OperatorRegistry::RegisterCommutativeOperatorForAllTypes(
    const std::string& oper, 
    OperatorRegistry::BinaryHandler handler) 
{
    RegisterCommutativeOperator<T, NullType>(oper, handler);
    RegisterCommutativeOperator<T, Int>(oper, handler);
    RegisterCommutativeOperator<T, Float>(oper, handler);
    RegisterCommutativeOperator<T, String>(oper, handler);
    RegisterCommutativeOperator<T, Bool>(oper, handler);
    RegisterCommutativeOperator<T, Function>(oper, handler);
}

template<CoreValueType T>
void OperatorRegistry::RegisterAllComparisonOps() {
    const auto cmp = [](auto op) {
        return [op](const Value& left, const Value& right) {
            return op(left.Get<T>(), right.Get<T>());
        };
    };
    
    RegisterBinaryOper<T, T>("==", cmp(std::equal_to{}));
    RegisterBinaryOper<T, T>("!=", cmp(std::not_equal_to{}));
    RegisterBinaryOper<T, T>("<", cmp(std::less{}));
    RegisterBinaryOper<T, T>("<=", cmp(std::less_equal{}));
    RegisterBinaryOper<T, T>(">", cmp(std::greater{}));
    RegisterBinaryOper<T, T>(">=", cmp(std::greater_equal{}));
}

} // namespace itmoscript
