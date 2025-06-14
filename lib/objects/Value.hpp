#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <map>
#include <ostream>
#include <type_traits>
#include <concepts>

#include "ast/AST.hpp"

#include "Function.hpp"

namespace itmoscript {

using NullType = std::monostate; // Represents the null value.
using Int = int64_t;             // Integer type used in the language.
using Float = double;            // Floating-point type used in the language.
using Bool = bool;               // Bool type used in the language.

class ListObject;
using List = std::shared_ptr<ListObject>; // List type used in the language.
using String = std::shared_ptr<std::string>; // String type used in the language.

/**
 * @brief Concept to constrain core language types for Value class.
 */
template<typename T>
concept CoreValueType = 
    std::same_as<T, NullType> ||
    std::same_as<T, Int> || std::convertible_to<T, Int> ||
    std::same_as<T, Float> || std::convertible_to<T, Float> ||
    std::same_as<T, String> ||
    std::same_as<T, Bool> ||
    std::same_as<T, Function> ||
    std::same_as<T, List>;

/**
 * @brief Concept to contrain types that are not copied but rather passed by reference.
 * Current reference value types are: List, String, Function.
 * 
 * If a ReferenceValueType is inserted into an array, it's getting copied.
 */
template<typename T>
concept ReferenceValueType =
    std::same_as<T, List> ||
    std::same_as<T, String> ||
    std::same_as<T, Function>;

/**
 * @brief Concept to constrain supported numeric types for Value class.
 * Current supported numeric types are: Int, Float.
 */
template<typename T>
concept NumericValueType = 
    std::same_as<T, Int> || std::convertible_to<T, Int> ||
    std::same_as<T, Float> || std::convertible_to<T, Float>;

/** 
 * @brief Enum representing all possible types stored in a Value. 
 * Integer value assigned to the enum constants are the type priority used
 * in comparison of 2 Values of different types (Ints and Floats are compared separately).
 * */
enum class ValueType {
    kInt = 1,
    kFloat = 2,
    kBool = 3,
    kString = 4,
    kList = 5,
    kFunction = 6,
    kNullType = 7,
};

inline const std::string kUnknownTypeName = "<UnknownType>";

/** @brief Hasher for std::pair<ValueType, ValueType>. Used for std::unordered_map of type converters. */
struct ValueTypePairHash {
    size_t operator()(const std::pair<ValueType, ValueType>& p) const {
        return std::hash<ValueType>()(p.first) ^ 
            (std::hash<ValueType>()(p.second) << 1);
    }
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
     * @tparam T Must satisfy CoreValueType concept.
     * @param val The value to store.
     */
    template<CoreValueType T>
    Value(const T& val)
        : data_(val) {}

    /**
     * @brief Constructs a Value with NullType.
     */
    Value() = default;

    /** @brief Returns the ValueType enum corresponding to the stored type. */
    ValueType GetType() const;

    bool IsOfType(ValueType type) const;
    
    /**
     * @brief Checks if the stored value is of type T.
     * @tparam T The type to check. Must satisfy CoreValueType concept.
     */
    template<CoreValueType T>
    bool IsOfType() const {
        return std::holds_alternative<T>(data_);
    }

    /**
     * @brief Checks if the stored value is truthy according to language rules.
     * nil and false are falsy, nonzero numbers and non-empty strings are truthy, all functions are truthy.
     */
    bool IsTruphy() const;
    
    /**
     * @brief Returns the stored value casted to type T.
     * 
     * @tparam T Must satisfy CoreValueType.
     * @return Const reference to the stored value.
     * @throws std::bad_variant_access if type does not match.
     */
    template<CoreValueType T>
    const T& Get() const {
        return std::get<T>(data_);
    }

    template<CoreValueType T>
    T& Get() {
        return std::get<T>(data_);
    }

    /** @tparam T Must satisfy CoreValueType. */
    template<CoreValueType T>
    Value& operator=(T value) {
        data_ = std::move(value);
        return *this;
    }

    /**
     * @brief Converts the stored value to string.
     * Can be used for debugging or printing.
     */
    std::string ToString() const;

    /**
     * @brief Puts string representation of the stored value to the stream.
     * Directly uses ToString().
     */
    friend std::ostream& operator<<(std::ostream& stream, const Value& value);

    /**
     * @brief Checks for equality with other Value object
     * by comparing the stored types and data.
     */
    bool operator==(const Value& other) const;

    /**
     * @brief Returns string representation of the stored value's type.
     * Can be used for debugging or dynamic type check in the language.
     */
    const std::string& GetTypeName() const;

    /**
     * @brief Checks if the value is less than the other value.
     * 
     * @details Specifically, if operands have different types, the priority of the type
     * goes as a key of comparison (see ValueType for more info).
     * 
     * The rules of comparison when types are the same are:
     * 1. Any two NullTypes are equal
     * 2. Int and Float - just a number comparison
     * 3. String - compared lexicographically (using std::string::operator<)
     * 4. Bool - false < true
     * 5. Functions are incomparable, so the operator simply returns false. Don't sort functions.
     * 6. Lists are compared using std::vector::operator<
     */
    bool operator<(const Value& other) const;

    /**
     * @brief Checks if the current type is a reference type.
     * See ReferenceValueType for more info.
     */
    bool IsReferenceType() const;

    /**
     * @brief Returns copy of the value.
     * If current type is ReferenceValueType, returns copy of the current data,
     * not the reference.
     */
    Value GetCopy() const;

private:
    using Type = std::variant<
        NullType,
        Int,
        Float,
        String,
        Bool,
        Function,
        List
    >;

    Type data_ = NullType{};
};

/** @brief Mapping from ValueType enum to string names for debugging/logging. */
inline const std::map<ValueType, std::string> kValueTypeNames = {
    {ValueType::kNullType, "NullType"},
    {ValueType::kInt, "Int"},
    {ValueType::kFloat, "Float"},
    {ValueType::kString, "String"},
    {ValueType::kBool, "Bool"},
    {ValueType::kFunction, "Function"},
    {ValueType::kList, "List"},
};

/** @brief Set containing all reference types. ReferenceValueType for more info. */
inline const std::set<ValueType> kReferenceTypes = {
    ValueType::kString,
    ValueType::kFunction,
    ValueType::kList,
};

/** 
 * @brief Returns string representation of the type (type name).
 * Should always be used instead of direct usage of kValueTypeNames.
 * */
const std::string& GetTypeName(ValueType type);

template<CoreValueType T>
ValueType GetType() {
    if constexpr (std::is_same_v<T, Int>) return ValueType::kInt;
    if constexpr (std::is_same_v<T, Float>) return ValueType::kFloat;
    if constexpr (std::is_same_v<T, Bool>) return ValueType::kBool;
    if constexpr (std::is_same_v<T, Function>) return ValueType::kFunction;
    if constexpr (std::is_same_v<T, List>) return ValueType::kList;
    if constexpr (std::is_same_v<T, String>) return ValueType::kString;
    if constexpr (std::is_same_v<T, NullType>) return ValueType::kNullType;
}

template<CoreValueType T>
const std::string& GetTypeName() {
    return GetTypeName(GetType<T>());
}

/**
 * @brief Creates a shared_ptr to the given InnerType.
 * The given value gets moved to the constructor of the given type.
 */
template<typename InnerType>
std::shared_ptr<InnerType> CreateRefValue(InnerType& val) {
    return std::make_shared<InnerType>(std::move(val));
}

static String CreateString(std::string val) {
    return CreateRefValue(val);
}

} // namespace itmoscript
