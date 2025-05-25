#include "TypeSystem.hpp"

namespace itmoscript {

bool TypeSystem::CanConvert(ValueType from, ValueType to) const {
    auto key = std::make_pair(from, to);
    return from == to || converters_.contains(key);
}

std::optional<Value> TypeSystem::TryConvert(const Value& v, ValueType target) const {
    if (target == v.GetType()) return v;
    else if (target == ValueType::kBool) return v.IsTruphy(); // added this to avoid 
                                                              // registering conversions for all types

    auto key = std::make_pair(v.GetType(), target);
    if (converters_.contains(key)) {
        return std::invoke(converters_.at(key), v);
    }

    return std::nullopt;
}

std::optional<ValueType> TypeSystem::FindCommonType(const Value& a, const Value& b) const {
    static const std::vector<ValueType> type_priority = {
        ValueType::kFloat,
        ValueType::kInt,
        ValueType::kBool
    };
    
    for (auto t : type_priority) {
        if (CanConvert(a.GetType(), t) && 
            CanConvert(b.GetType(), t)) {
            return t;
        }
    }
    
    return std::nullopt;
}

} // namespace itmoscript
