#include "Value.hpp"

namespace ItmoScript {

bool Value::IsNullType() const {
    return IsOfType(ValueType::kNullType);
}

bool Value::IsInt() const {
    return IsOfType(ValueType::kInt);
}

bool Value::IsFloat() const {
    return IsOfType(ValueType::kFloat);
}

bool Value::IsString() const {
    return IsOfType(ValueType::kString);
}

bool Value::IsBool() const {
    return IsOfType(ValueType::kBool);
}

bool Value::IsFunction() const {
    return IsOfType(ValueType::kFunction);
}

std::string Value::ToString() const {
    switch (GetType()) {
        case ValueType::kNullType:
            return "nil";
        case ValueType::kInt:
            return std::to_string(GetValue<Int>());
        case ValueType::kFloat:
            return std::to_string(GetValue<Float>());
        case ValueType::kString:
            return GetValue<String>();
        case ValueType::kBool:
            return GetValue<Bool>() ? "true" : "false";
        case ValueType::kFunction:
            return "Function";
        default:
            return "<UnknownType>";
    }
}

std::ostream& operator<<(std::ostream& stream, const Value& value) {
    return stream << value.ToString();
}

ValueType Value::GetType() const {
    if (IsInt()) {
        return ValueType::kInt;
    } else if (IsFloat()) {
        return ValueType::kFloat;
    } else if (IsString()) {
        return ValueType::kString;
    } else if (IsBool()) {
        return ValueType::kBool;
    } else if (IsFunction()) {
        return ValueType::kFunction;
    }

    return ValueType::kNullType;
}

bool Value::IsOfType(ValueType type) const {
    switch (type) {
        case ValueType::kNullType:
            return std::holds_alternative<NullType>(data_);
        case ValueType::kInt:
            return std::holds_alternative<Int>(data_);
        case ValueType::kFloat:
            return std::holds_alternative<Float>(data_);
        case ValueType::kString:
            return std::holds_alternative<String>(data_);
        case ValueType::kBool:
            return std::holds_alternative<Bool>(data_);
        case ValueType::kFunction:
            return std::holds_alternative<Function>(data_);
        default:
            return false;
    }
}

} // namespace ItmoScript
