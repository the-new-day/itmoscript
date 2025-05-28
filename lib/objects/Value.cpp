#include "Value.hpp"
#include "Function.hpp"
#include "List.hpp"
#include "utils.hpp"

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
            return !Get<String>()->empty();
        case ValueType::kBool:
            return Get<Bool>();
        case ValueType::kFunction:
            return true;
        case ValueType::kList:
            return Get<List>()->size() != 0;
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
            return '"' + *Get<String>() + '"';
        case ValueType::kBool:
            return Get<Bool>() ? "true" : "false";
        case ValueType::kFunction:
            return std::format(
                "<Function object>({})",
                utils::Join<std::shared_ptr<ast::Identifier>, std::string>(
                    Get<Function>().parameters(), 
                    ", ", 
                    [](const std::shared_ptr<ast::Identifier>& ident) { return ident->name; }
                )
            );
        case ValueType::kList:
            return std::format(
                "[{}]",
                utils::Join<Value, std::string>(
                    Get<List>()->data(), 
                    ", ", 
                    [](const Value& value) { return value.ToString(); }
                )
            );
        default:
            return kUnknownTypeName;
    }
}

bool Value::operator==(const Value& other) const {
    if (type() == ValueType::kList) {
        return other.type() == ValueType::kList
            && Get<List>()->data() == other.Get<List>()->data();
    } else if (type() == ValueType::kString) {
        return other.type() == ValueType::kString
            && *Get<String>() == *other.Get<String>();
    } else if (type() == ValueType::kFunction) {
        return other.type() == ValueType::kFunction
            && Get<Function>() == other.Get<Function>();
    }
    
    return type() == other.type() && data_ == other.data_;
}

const std::string& Value::GetTypeName() const {
    return itmoscript::GetTypeName(type());
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
    } else if (IsOfType<List>()) {
        return ValueType::kList;
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
        case ValueType::kList:
            return std::holds_alternative<List>(data_);
        default:
            return false;
    }
}

const std::string& GetTypeName(ValueType type) {
    if (kValueTypeNames.contains(type)) {
        return kValueTypeNames.at(type);
    } else {
        return kUnknownTypeName;
    }
}

} // namespace itmoscript
