#include "TypeSystem.hpp"

namespace itmoscript {

bool TypeSystem::CanConvert(ValueType from, ValueType to) const {
    auto key = std::make_pair(from, to);
    return from == to || converters_.contains(key);
}

std::optional<Value> TypeSystem::TryConvert(const Value& v, ValueType target) const {
    if (target == v.type()) return v;

    auto key = std::make_pair(v.type(), target);
    if (converters_.contains(key)) {
        return std::invoke(converters_.at(key), v);
    }

    return std::nullopt;
}

std::optional<ValueType> TypeSystem::FindCommonType(ValueType a, ValueType b) const {
    if (a == b) return a;

    static const std::vector<ValueType> type_priority = {
        ValueType::kFloat,
        ValueType::kInt,
    };
    
    for (auto t : type_priority) {
        if (CanConvert(a, t) && CanConvert(b, t)) {
            return t;
        }
    }
    
    return std::nullopt;
}

} // namespace itmoscript
