#include "Value.hpp"

namespace itmoscript {

bool Value::IsTruphy() const {
    switch (type()) {
        case ValueType::kNullType:
            return false;
        case ValueType::kInt:
            return Get<Int>() != 0;
        case ValueType::kFloat:
            return Get<Float>() != 0;
        case ValueType::kString:
            return !Get<String>().empty();
        case ValueType::kBool:
            return Get<Bool>();
        case ValueType::kFunction:
            return true;
        default:
            return false;
    }
}

std::string Value::ToString() const {
    switch (type()) {
        case ValueType::kNullType:
            return "nil";
        case ValueType::kInt:
            return std::to_string(Get<Int>());
        case ValueType::kFloat:
            return std::format("{}", Get<Float>());
        case ValueType::kString:
            return Get<String>();
        case ValueType::kBool:
            return Get<Bool>() ? "true" : "false";
        case ValueType::kFunction:
            return "Function";
        default:
            return "<UnknownType>";
    }
}

bool Value::operator==(const Value& other) const {
    return type() == other.type() && data_ == other.data_;
}

const std::string Value::GetTypeName() const {
    return kValueTypeNames.at(type());
}

std::ostream& operator<<(std::ostream& stream, const Value& value) {
    return stream << value.ToString();
}

ValueType Value::type() const {
    if (IsOfType<Int>()) {
        return ValueType::kInt;
    } else if (IsOfType<Float>()) {
        return ValueType::kFloat;
    } else if (IsOfType<String>()) {
        return ValueType::kString;
    } else if (IsOfType<Bool>()) {
        return ValueType::kBool;
    } else if (IsOfType<Function>()) {
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

} // namespace itmoscript
