#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "Value.hpp"
#include "TypeSystem.hpp"

namespace itmoscript {

/**
 * @class OperatorRegistry
 * @brief Stores operator handlers for specific types.
 * Allows registering and retrieving operator handlers for unary and binary operators.
 */
class OperatorRegistry {
public:
    /**
     * @brief Functional object responsible for handling binary operators.
     * Used for operators in expressions like "5 + 5".
     */
    using BinaryHandler = std::function<Value(const Value&, const Value&)>;
    
    /**
     * @brief Functional object responsible for handling unary operators.
     * Used for operators in expressions like "!x", "-x"
     */
    using UnaryHandler = std::function<Value(const Value&)>;

    template<CoreValueType Right>
    void RegisterUnaryOper(const std::string& oper, UnaryHandler handler);

    template<CoreValueType Left, CoreValueType Right>
    void RegisterBinaryOper(const std::string& oper, BinaryHandler handler);

    /**
     * @brief Registers commutative operator, i.e. the operator that doesn't
     * depend on the order of the operands, like "+" and "*".
     * @tparam T Value type 1
     * @tparam U Value type 2
     * 
     * For example, if oper is "+", the operation will be registered for T + U and U + T
     */
    template<CoreValueType T, CoreValueType U>
    void RegisterCommutativeOperator(const std::string& oper, BinaryHandler handler);


    /**
     * @brief Registers commutative operator for all types.
     * 
     * For example, if oper is "+", the operation will be registered for T + U and U + T,
     * where U is NullType, Int, Float etc. - for all core types.
     */
    template<CoreValueType T>
    void RegisterCommutativeOperatorForAllTypes(const std::string& oper, BinaryHandler handler);

    /**
     * @brief Registers commutative operator for all pairs of types.
     * 
     * Specifically, calls RegisterCommutativeOperatorForAllTypes<U>(oper, handler)
     * for all U that satisfy CoreValueType concept.
     */
    void RegisterCommutativeOperatorForAllPairsOfTypes(const std::string& oper, BinaryHandler handler);

    /**
     * @brief Registers unary operator for all types.
     * 
     * Specifically, calls RegisterUnaryOper<U>(oper, handler)
     * for all U that satisfy CoreValueType concept.
     */
    void RegisterUnaryOperatorForAllTypes(const std::string& oper, UnaryHandler handler);

    /**
     * @brief Registers all comparison operators for specified type.
     *
     * All comparison operators are: ==, != , <, <=, >, >=
     */
    template<CoreValueType T>
    void RegisterAllComparisonOps();

    /**
     * @brief Finds exact handler for given binary operator with given left and right types.
     * @return The handler if it was found for given types and operator, std::nullopt otherwise.
     */
    std::optional<BinaryHandler> FindExactHandler(const std::string& oper, ValueType left, ValueType right);

    /**
     * @brief Finds exact handler for given unary operator with given value type.
     * @return The handler if it was found for given type and operator, std::nullopt otherwise.
     */
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
