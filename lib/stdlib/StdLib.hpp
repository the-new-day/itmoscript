#pragma once

#include "objects/Value.hpp"

#include "evaluation/exceptions/ParametersCountError.hpp"
#include "exceptions/ArgumentTypeError.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <concepts>
#include <expected>

namespace itmoscript {

namespace stdlib {

using BuiltInFunction = std::function<Value(const std::vector<Value>&, Token, const CallStack&)>;

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
 * @brief Gets function that handles specified Values
 * and returns a wrapper over it which gets Token, CallStack and std::vector<Value>.
 * 
 * The wrapper is also validating the number of arguments passed and throws an exception.
 * Value types are not validated.
 * 
 * @param fn Function to handle the value
 * @param arg_num Amount of arguments to expect.
 * If the amount of arguments passed to the returned lambda is not equal to arg_num,
 * ParametersCountError will be thrown by the wrapper.
 */
template<typename F> requires std::invocable<F, const std::vector<Value>&, Token, const CallStack&>
auto MakeBuiltin(F fn, size_t arg_num) {
    return [fn, arg_num](const std::vector<Value>& args,
                         Token from,
                         const CallStack& stack) -> Value {
        if (args.size() != arg_num) {
            throw lang_exceptions::ParametersCountError{from, stack, arg_num, args.size()};
        }

        return std::invoke(fn, args, from, stack);
    };
}

template<typename ErrorType, typename... Args>
void ThrowError(Token from, const CallStack& call_stack, Args&&... args) {
    throw ErrorType{std::move(from), call_stack, std::forward<Args>(args)...};
}

void ThrowArgumentTypeError(
    Token from, 
    const CallStack& call_stack, 
    size_t idx, 
    ValueType given_type, 
    const std::string& expected
);

template<typename T>
void AssertType(
    const Value& val,
    size_t idx, 
    Token from, 
    const CallStack& call_stack
) {
    if (!val.IsOfType<T>()) {
        ThrowArgumentTypeError(std::move(from), call_stack, idx, val.type(), GetTypeName<T>());
    }
}
    
} // namespace stdlib
    
} // namespace itmoscript
