#include "TypeSystem.hpp"

namespace ItmoScript {

TypeSystem::TypeSystem() {
    RegisterConversion<Int, Float>([](Int value) { return static_cast<Float>(value); });
    RegisterConversion<Float, Int>([](Float value) { return static_cast<Int>(value); });
}

bool TypeSystem::CanConvert(std::type_index from, std::type_index to) const {
    auto key = std::make_pair(from, to);
    return from == to || converters_.contains(key);
}

std::optional<Value> TypeSystem::TryConvert(const Value& v, std::type_index target) const {
    if (target == v.GetTypeIndex()) return v;

    auto key = std::make_pair(v.GetTypeIndex(), target);
    if (converters_.contains(key)) {
        return std::invoke(converters_.at(key), v);
    }

    return std::nullopt;
}

std::optional<std::type_index> TypeSystem::FindCommonType(const Value& a, const Value& b) const {
    static const std::vector<std::type_index> type_priority = {
        typeid(Float),
        typeid(Int),
        typeid(String),
        typeid(Bool),
    };
    
    for (auto t : type_priority) {
        if (CanConvert(a.GetTypeIndex(), t) && 
            CanConvert(b.GetTypeIndex(), t)) {
            return t;
        }
    }
    
    return std::nullopt;
}

} // namespace ItmoScript
