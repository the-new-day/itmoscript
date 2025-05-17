#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <map>

namespace ItmoScript {

struct Function {};

using NullType = std::monostate;
using Int = int64_t;
using Float = double;
using String = std::string;
using Bool = bool;
//using Function = Function;

using Value = std::variant<
    NullType,
    Int,
    Float,
    String,
    Bool,
    Function
>;

enum class ValueType {
    kNullType,
    kInt,
    kFloat,
    kString,
    kBool,
    kFunction
};

bool IsNullType(const Value& value);
bool IsInt(const Value& value);
bool IsFloat(const Value& value);
bool IsString(const Value& value);
bool IsBool(const Value& value);
bool IsFunction(const Value& value);
ValueType GetType(const Value& value);

template<typename T>
T GetValue(const Value& value) {
    return std::get<T>(value);
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
