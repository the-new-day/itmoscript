#include "Value.hpp"

namespace ItmoScript {

bool IsNullType(const Value& value) {
    return std::holds_alternative<std::monostate>(value);
}

bool IsInt(const Value& value) {
    return std::holds_alternative<int64_t>(value);
}

bool IsFloat(const Value& value) {
    return std::holds_alternative<double>(value);
}

bool IsString(const Value& value) {
    return std::holds_alternative<std::string>(value);
}

bool IsBool(const Value& value) {
    return std::holds_alternative<bool>(value);
}

bool IsFunction(const Value& value) {
    return std::holds_alternative<Function>(value);
}

ValueType GetType(const Value& value) {
    if (IsNullType(value)) {
        return ValueType::kNullType;
    } else if (IsInt(value)) {
        return ValueType::kInt;
    } else if (IsFloat(value)) {
        return ValueType::kFloat;
    } else if (IsString(value)) {
        return ValueType::kString;
    } else if (IsBool(value)) {
        return ValueType::kBool;
    } else if (IsFunction(value)) {
        return ValueType::kFunction;
    }

    return ValueType::kNullType;
}
    
} // namespace ItmoScript

