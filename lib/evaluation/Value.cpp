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

bool Value::IsTruphy() const {
    switch (GetType()) {
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
    switch (GetType()) {
        case ValueType::kNullType:
            return "nil";
        case ValueType::kInt:
            return std::to_string(Get<Int>());
        case ValueType::kFloat:
            return std::to_string(Get<Float>());
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
    return GetType() == other.GetType() && data_ == other.data_;
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

std::type_index Value::GetTypeIndex() const {
    if (IsInt()) {
        return typeid(Int);
    } else if (IsFloat()) {
        return typeid(Float);
    } else if (IsString()) {
        return typeid(String);
    } else if (IsBool()) {
        return typeid(Bool);
    } else if (IsFunction()) {
        return typeid(Function);
    }
    
    return typeid(NullType);
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
