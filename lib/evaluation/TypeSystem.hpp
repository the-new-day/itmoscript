#pragma once

#include <functional>
#include <unordered_map>
#include <type_traits>

#include "Value.hpp"
#include "utils.hpp"

namespace itmoscript {

class TypeSystem {
public:
    TypeSystem();

    template<CoreValueType From, CoreValueType To>
    using Convertion = std::function<To(const From&)>;

    bool CanConvert(ValueType from, ValueType to) const;
    std::optional<Value> TryConvert(const Value& v, ValueType target) const;
    std::optional<ValueType> FindCommonType(const Value& a, const Value& b) const;

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
