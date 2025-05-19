#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <map>
#include <ostream>
#include <typeindex>

namespace ItmoScript {

struct Function {
    bool operator==(const Function& other) const {
        return false;
    }
};

using NullType = std::monostate;
using Int = int64_t;
using Float = double;
using String = std::string;
using Bool = bool;

enum class ValueType {
    kNullType,
    kInt,
    kFloat,
    kString,
    kBool,
    kFunction
};

class Value {
public:
    template<typename T>
    Value(const T& val)
        : data_(val) {}

    Value() = default;

    ValueType GetType() const;
    std::type_index GetTypeIndex() const;
    bool IsOfType(ValueType type) const;
    
    template<typename T>
    bool IsOfType() const;

    bool IsNullType() const;
    bool IsInt() const;
    bool IsFloat() const;
    bool IsString() const;
    bool IsBool() const;
    bool IsFunction() const;

    // Checks if contained value can be converted to true following the language rules.
    bool IsTruphy() const;
    
    // TODO: make T a concept to prevent wrong types
    template<typename T>
    T GetValue() const {
        return std::get<T>(data_);
    }

    template<typename T>
    void Set(const T& value) {
        data_ = value;
    }

    template<typename T>
    Value& operator=(const T& value) {
        Set(value);
        return *this;
    }

    std::string ToString() const;
    friend std::ostream& operator<<(std::ostream& stream, const Value& value);

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

template<typename T>
bool Value::IsOfType() const {
    return std::holds_alternative<T>(data_);
}

const std::map<ValueType, std::string> kValueTypeNames{
    {ValueType::kNullType, "NullType"},
    {ValueType::kInt, "Int"},
    {ValueType::kFloat, "Float"},
    {ValueType::kString, "String"},
    {ValueType::kBool, "Bool"},
    {ValueType::kFunction, "Function"},
};

} // namespace ItmoScript
