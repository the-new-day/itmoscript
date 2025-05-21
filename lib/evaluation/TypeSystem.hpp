#pragma once

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <type_traits>

#include "Value.hpp"
#include "utils.hpp"

namespace ItmoScript {

class TypeSystem {
public:
    TypeSystem();

    template<typename From, typename To>
    using Convertion = std::function<To(const From&)>;

    bool CanConvert(std::type_index from, std::type_index to) const;
    std::optional<Value> TryConvert(const Value& v, std::type_index target) const;
    std::optional<std::type_index> FindCommonType(const Value& a, const Value& b) const;

    template <typename From, typename To>
    void RegisterConversion(Convertion<From, To> func) {
        converters_[{typeid(From), typeid(To)}] = [func](const Value& v) { 
            return std::invoke(func, v.Get<From>());
        };
    }

private:
    using Converter = std::function<Value(const Value&)>;
    
    std::unordered_map<
        std::pair<std::type_index, std::type_index>,
        Converter,
        Utils::TypePairHash
    > converters_;
};
    
} // namespace ItmoScript
