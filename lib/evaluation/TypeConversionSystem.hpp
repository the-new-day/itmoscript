#pragma once

#include <functional>
#include <unordered_map>
#include <type_traits>

#include "objects/Value.hpp"
#include "utils.hpp"

namespace itmoscript {

/**
 * @class TypeConversionSystem
 * @brief Holds the type conversion rules and allows 
 * implicit type conversions upon request.
 */
class TypeConversionSystem {
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
     * @brief Tries to find the common type - the type to which values both types are convertible.
     * @return The common type if the conversion is found, nullopt otherwise.
     * @details If a and b are of the same type, returns it. Otherwise, does the following.
     * Goes through types allowed for implicit conversion in the priority order, namely:
     * 1. Int
     * 2. Float
     * 
     * If the direct conversion of both values to one of the types above is found, returns the type.
     * If no conversion is found, returns nullopt.
     */
    std::optional<ValueType> FindCommonType(ValueType a, ValueType b) const;

    /**
     * @brief Registers a conversion from type to type.
     * @tparam From Type to convert from. Must satisfy the CoreValueType concept.
     * @tparam To Type to convert to. Must satisfy the CoreValueType concept.
     */
    template <CoreValueType From, CoreValueType To>
    void RegisterConversion(Convertion<From, To> func) {
        converters_[{GetType<From>(), GetType<To>()}] = [func](const Value& v) { 
            return std::invoke(func, v.Get<From>());
        };
    }

private:
    using Converter = std::function<Value(const Value&)>;
    
    std::unordered_map<
        std::pair<ValueType, ValueType>,
        Converter,
        ValueTypePairHash
    > converters_;
};
    
} // namespace itmoscript
