#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "objects/Value.hpp"
#include "TypeConversionSystem.hpp"

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
    void RegisterUnaryOper(TokenType oper, UnaryHandler handler);

    template<CoreValueType Left, CoreValueType Right>
    void RegisterBinaryOper(TokenType oper, BinaryHandler handler);

    /**
     * @brief Registers commutative operator, i.e. the operator that doesn't
     * depend on the order of the operands, like "+" and "*".
     * @tparam T Value type 1
     * @tparam U Value type 2
     * 
     * For example, if oper is "+", the operation will be registered for T + U and U + T
     */
    template<CoreValueType T, CoreValueType U>
    void RegisterCommutativeOperator(TokenType oper, BinaryHandler handler);


    /**
     * @brief Registers commutative operator for all types.
     * 
     * For example, if oper is "+", the operation will be registered for T + U and U + T,
     * where U is NullType, Int, Float etc. - for all core types.
     */
    template<CoreValueType T>
    void RegisterCommutativeOperatorForAllTypes(TokenType oper, BinaryHandler handler);

    /**
     * @brief Registers commutative operator for all pairs of types.
     * 
     * Specifically, calls RegisterCommutativeOperatorForAllTypes<U>(oper, handler)
     * for all U that satisfy CoreValueType concept.
     */
    void RegisterCommutativeOperatorForAllPairsOfTypes(TokenType oper, BinaryHandler handler);

    /**
     * @brief Registers unary operator for all types.
     * 
     * Specifically, calls RegisterUnaryOper<U>(oper, handler)
     * for all U that satisfy CoreValueType concept.
     */
    void RegisterUnaryOperatorForAllTypes(TokenType oper, UnaryHandler handler);

    /**
     * @brief Registers all comparison operators for specified type.
     *
     * All comparison operators are: ==, != , <, <=, >, >=
     */
    template<CoreValueType T>
    void RegisterAllComparisonOps();

    /**
     * @brief Registers all comparison operators for specified 
     * reference type (e.g. list, string, function).
     *
     * All comparison operators are: ==, != , <, <=, >, >=
     */
    template<CoreValueType T>
    void RegisterAllComparisonOpsForRefType();

    /**
     * @brief Finds exact handler for given binary operator with given left and right types.
     * @return The handler if it was found for given types and operator, std::nullopt otherwise.
     */
    std::optional<BinaryHandler> FindExactHandler(TokenType oper, ValueType left, ValueType right);

    /**
     * @brief Finds exact handler for given unary operator with given value type.
     * @return The handler if it was found for given type and operator, std::nullopt otherwise.
     */
    std::optional<UnaryHandler> FindExactHandler(TokenType oper, ValueType type);

private:
    std::unordered_map<
        TokenType, 
        std::unordered_map<
            std::pair<ValueType, ValueType>,
            BinaryHandler,
            ValueTypePairHash
        >
    > binary_ops_;

    std::unordered_map<
        TokenType, 
        std::unordered_map<
            ValueType,
            UnaryHandler
        >
    > unary_ops_;
};

template<CoreValueType Right>
void OperatorRegistry::RegisterUnaryOper(TokenType oper, UnaryHandler handler) {
    unary_ops_[oper][GetType<Right>()] = handler;
}

template<CoreValueType Left, CoreValueType Right>
void OperatorRegistry::RegisterBinaryOper(TokenType oper, BinaryHandler handler) {
    binary_ops_[oper][{GetType<Left>(), GetType<Right>()}] = handler;
}

template<CoreValueType T, CoreValueType U>
void OperatorRegistry::RegisterCommutativeOperator(TokenType oper, BinaryHandler handler) {
    binary_ops_[oper][{GetType<T>(), GetType<U>()}] = handler;
    binary_ops_[oper][{GetType<U>(), GetType<T>()}] = handler;
}

template<CoreValueType T>
void OperatorRegistry::RegisterCommutativeOperatorForAllTypes(
    TokenType oper, 
    OperatorRegistry::BinaryHandler handler) 
{
    RegisterCommutativeOperator<T, NullType>(oper, handler);
    RegisterCommutativeOperator<T, Int>(oper, handler);
    RegisterCommutativeOperator<T, Float>(oper, handler);
    RegisterCommutativeOperator<T, String>(oper, handler);
    RegisterCommutativeOperator<T, Bool>(oper, handler);
    RegisterCommutativeOperator<T, Function>(oper, handler);
    RegisterCommutativeOperator<T, List>(oper, handler);
}

template<CoreValueType T>
void OperatorRegistry::RegisterAllComparisonOps() {
    const auto cmp = [](auto op) {
        return [op](const Value& left, const Value& right) {
            return op(left.Get<T>(), right.Get<T>());
        };
    };
    
    RegisterBinaryOper<T, T>(TokenType::kEqual, cmp(std::equal_to{}));
    RegisterBinaryOper<T, T>(TokenType::kNotEqual, cmp(std::not_equal_to{}));
    RegisterBinaryOper<T, T>(TokenType::kLess, cmp(std::less{}));
    RegisterBinaryOper<T, T>(TokenType::kLessOrEqual, cmp(std::less_equal{}));
    RegisterBinaryOper<T, T>(TokenType::kGreater, cmp(std::greater{}));
    RegisterBinaryOper<T, T>(TokenType::kGreaterOrEqual, cmp(std::greater_equal{}));
}

template<CoreValueType T>
void OperatorRegistry::RegisterAllComparisonOpsForRefType() {
    const auto cmp = [](auto op) {
        return [op](const Value& left, const Value& right) {
            return op(*left.Get<T>(), *right.Get<T>());
        };
    };
    
    RegisterBinaryOper<T, T>(TokenType::kEqual, cmp(std::equal_to{}));
    RegisterBinaryOper<T, T>(TokenType::kNotEqual, cmp(std::not_equal_to{}));
    RegisterBinaryOper<T, T>(TokenType::kLess, cmp(std::less{}));
    RegisterBinaryOper<T, T>(TokenType::kLessOrEqual, cmp(std::less_equal{}));
    RegisterBinaryOper<T, T>(TokenType::kGreater, cmp(std::greater{}));
    RegisterBinaryOper<T, T>(TokenType::kGreaterOrEqual, cmp(std::greater_equal{}));
}

} // namespace itmoscript
