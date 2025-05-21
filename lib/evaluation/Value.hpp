#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <map>
#include <ostream>
#include <typeindex>
#include <concepts>

namespace ItmoScript {

/**
 * @brief Represents a function value in the language.
 * Two Function instances are equal only if they are the exact same object.
 */
struct Function {
    bool operator==(const Function& other) const {
        return this == &other;
    }
};

using NullType = std::monostate; // Represents the null value.
using Int = int64_t;             // Integer type used in the language.
using Float = double;            // Floating-point type used in the language.
using String = std::string;      // String type used in the language.
using Bool = bool;               // Bool type used in the language.

/**
 * @brief Concept to constrain supported types for Value class.
 * 
 * Supports exact types and types convertible to Int or Float.
 * This enables implicit conversions like int -> int64_t and float -> double.
 */
template<typename T>
concept SupportedValueType = 
    std::same_as<T, NullType> ||
    std::same_as<T, Int> || std::convertible_to<T, Int> ||
    std::same_as<T, Float> || std::convertible_to<T, Float> ||
    std::same_as<T, String> ||
    std::same_as<T, Bool> ||
    std::same_as<T, Function>;

/**
 * @brief Concept to constrain supported numeric types for Value class.
 */
template<typename T>
concept NumericValueType = 
    std::same_as<T, Int> || std::convertible_to<T, Int> ||
    std::same_as<T, Float> || std::convertible_to<T, Float>;

/** @brief Enum representing all possible types stored in a Value. */
enum class ValueType {
    kNullType,
    kInt,
    kFloat,
    kString,
    kBool,
    kFunction
};

/**
 * @class Value
 * @brief Represents a dynamically-typed value in the ItmoScript language.
 * 
 * Can hold any of the supported types (NullType, Int, Float, String, Bool, Function).
 * Provides type-safe access and utilities for type checking and conversion.
 */
class Value {
public:
    /**
     * @brief Constructs a Value from any supported type or convertible type.
     * 
     * Allows implicit conversions from int, float, etc. to Value.
     * @tparam T Must satisfy SupportedValueType concept.
     * @param val The value to store.
     */
    template<SupportedValueType T>
    Value(const T& val)
        : data_(val) {}

    Value() = default;

    /** @brief Returns the ValueType enum corresponding to the stored type. */
    ValueType GetType() const;

    /** @brief Returns the std::type_index corresponding to the stored type. */
    std::type_index GetTypeIndex() const;

    bool IsOfType(ValueType type) const;
    
    /**
     * @brief Checks if the stored value is of type T.
     * @tparam T The type to check. Must satisfy SupportedValueType concept.
     */
    template<SupportedValueType T>
    bool IsOfType() const {
        return std::holds_alternative<T>(data_);
    }

    bool IsNullType() const;
    bool IsInt() const;
    bool IsFloat() const;
    bool IsString() const;
    bool IsBool() const;
    bool IsFunction() const;

    /**
     * @brief Checks if the stored value is truthy according to language rules.
     * nil and false are falsy, nonzero numbers and non-empty strings are truthy, all functions are truthy.
     */
    bool IsTruphy() const;
    
    /**
     * @brief Returns the stored value casted to type T.
     * 
     * @tparam T Must satisfy SupportedValueType.
     * @return The stored value.
     * @throws std::bad_variant_access if type does not match.
     */
    template<SupportedValueType T>
    T GetValue() const {
        return std::get<T>(data_);
    }

    /** @tparam T Must satisfy SupportedValueType. */
    template<SupportedValueType T>
    Value& operator=(const T& value) {
        data_ = value;
        return *this;
    }

    std::string ToString() const;
    friend std::ostream& operator<<(std::ostream& stream, const Value& value);

    /**
     * @brief Checks for equality with other Value object
     * by comparing the stored types and data.
     */
    bool operator==(const Value& other) const;

private:
    using Type = std::variant<
        NullType,
        Int,
        Float,
        String,
        Bool,
        Function
    >;

    Type data_;
};

/** @brief Mapping from ValueType enum to string names for debugging/logging. */
const std::map<ValueType, std::string> kValueTypeNames{
    {ValueType::kNullType, "NullType"},
    {ValueType::kInt, "Int"},
    {ValueType::kFloat, "Float"},
    {ValueType::kString, "String"},
    {ValueType::kBool, "Bool"},
    {ValueType::kFunction, "Function"},
};

} // namespace ItmoScript
