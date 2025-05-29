#pragma once

#include "objects/Value.hpp"

#include "evaluation/exceptions/ParametersCountError.hpp"
#include "exceptions/ArgumentTypeError.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <concepts>

namespace itmoscript {

namespace stdlib {

using BuiltInFunction = std::function<Value(Token, const CallStack&, const std::vector<Value>&)>;

class StdLib {
public:
    bool Has(const std::string& name) const;
    void Register(const std::string& name, BuiltInFunction func);
    Value Call( 
        const std::string& name, 
        const std::vector<Value>& args,
        Token from, 
        const CallStack& call_stack
    );

    void LoadDefault();

private:
    std::unordered_map<std::string, BuiltInFunction> functions_;
};

/**
 * @brief Gets function that handles a specified parameters
 * and returns a wrapper over it which gets Token, CallStack and std::vector<Value>.
 * 
 * The wrapper is also validating the number of arguments passed and their types
 * and throws an exception.
 */
template<CoreValueType... Ts, typename F> requires std::invocable<F, Ts...>
auto MakeBuiltin(F fn) {
    return [fn](Token from,
                const CallStack& stack,
                const std::vector<Value>& args) -> Value {
        if (args.size() != sizeof...(Ts)) {
            throw lang_exceptions::ParametersCountError{from, stack, sizeof...(Ts), args.size()};
        }
        
        size_t idx = 0;
        Value out;
        
        auto tup = std::make_tuple(
            ([&]() {
                using T = Ts;
                const Value& v = args[idx];
                if (!v.IsOfType<T>()) {
                    throw lang_exceptions::ArgumentTypeError{
                        from, stack, idx, v.type(), GetType<T>()
                    };
                }
                ++idx;
                return v.Get<T>();
            }())...
        );
        return std::apply(fn, tup);
    };
}
    
} // namespace stdlib
    
} // namespace itmoscript
