#pragma once

#include <functional>
#include <unordered_map>
#include <type_traits>

#include "Value.hpp"
#include "utils.hpp"

namespace itmoscript {

/**
 * @class TypeSystem
 * @brief Holds the type conversion rules and allows implicit type conversions upon request.
 */
class TypeSystem {
public:
    /**
     * @brief Describes a convertion rule.
     * A conversion rule is a function that get type From and return type To.
     * @tparam From Type to convert from. Must satisfy the CoreValueType concept.
     * @tparam To Type to convert to. Must satisfy the CoreValueType concept.
     */
    template<CoreValueType From, CoreValueType To>
    using Convertion = std::function<To(const From&)>;

    /** @brief Checks if a conversion from type to type has been registered. */
    bool CanConvert(ValueType from, ValueType to) const;

    /** @brief Converts value to the given type if a direct conversion exists. Otherwise, returns nullopt. */
    std::optional<Value> TryConvert(const Value& v, ValueType target) const;

    /**
     * @brief Tries to find the common type - the type to which both values are convertible.
     * @return The common type if the conversion is found, nullopt otherwise.
     * @details Goes through types allowed for implicit conversion in the priority order, namely:
     * 1. Int
     * 2. Float
     * 3. Bool
     * 
     * If the direct conversion of both values to one of the types above is found, returns the type.
     * If no conversion is found, returns nullopt.
     */
    std::optional<ValueType> FindCommonType(const Value& a, const Value& b) const;

    /**
     * @brief Registers a conversion from type to type.
     * @tparam From Type to convert from. Must satisfy the CoreValueType concept.
     * @tparam To Type to convert to. Must satisfy the CoreValueType concept.
     */
    template <CoreValueType From, CoreValueType To>
    void RegisterConversion(Convertion<From, To> func) {
        converters_[{GetValueType<From>(), GetValueType<To>()}] = [func](const Value& v) { 
            return std::invoke(func, v.Get<From>());
        };
    }

    /**
     * @brief Returns the ValueType enum value corresponding to the template parameter.
     * @tparam T The core value type. Must satisfy CoreValueType concept.
     */
    template<CoreValueType T>
    static ValueType GetValueType();

private:
    using Converter = std::function<Value(const Value&)>;
    
    std::unordered_map<
        std::pair<ValueType, ValueType>,
        Converter,
        ValueTypePairHash
    > converters_;
};

template<CoreValueType T>
ValueType TypeSystem::GetValueType() {
    if constexpr (std::is_same_v<T, Int>) return ValueType::kInt;
    if constexpr (std::is_same_v<T, Float>) return ValueType::kFloat;
    if constexpr (std::is_same_v<T, String>) return ValueType::kString;
    if constexpr (std::is_same_v<T, Bool>) return ValueType::kBool;
    if constexpr (std::is_same_v<T, Function>) return ValueType::kFunction;
    if constexpr (std::is_same_v<T, NullType>) return ValueType::kNullType;
}
    
} // namespace itmoscript
