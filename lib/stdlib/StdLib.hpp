#pragma once

#include "objects/Value.hpp"

#include "evaluation/exceptions/ParametersCountError.hpp"
#include "exceptions/ArgumentTypeError.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <concepts>
#include <iostream>

namespace itmoscript {

namespace stdlib {

using ValueHandlingFunction = std::function<Value(std::vector<Value>&, Token, const CallStack&)>;
using OutStreamHandlingFunction = std::function<Value(std::ostream&, std::vector<Value>&, Token, const CallStack&)>;
using InStreamHandlingFunction = std::function<Value(std::istream&, std::vector<Value>&, Token, const CallStack&)>;

class StdLib {
public:
    void Register(const std::string& name, ValueHandlingFunction func);
    void RegisterOutStreamHandlingFunc(const std::string& name, OutStreamHandlingFunction func);
    void RegisterInStreamHandlingFunc(const std::string& name, InStreamHandlingFunction func);
    
    bool HasValueHandlingFunc(const std::string& name) const;
    bool HasOutStreamHandlingFunc(const std::string& name) const;
    bool HasInStreamHandlingFunc(const std::string& name) const;

    bool Has(const std::string& name) const;
    
    Value Call( 
        const std::string& name, 
        std::vector<Value>& args,
        Token from, 
        const CallStack& call_stack
    );

    Value CallOutStreamHandlingFunc( 
        std::ostream& stream,
        const std::string& name, 
        std::vector<Value>& args,
        Token from, 
        const CallStack& call_stack
    );

    Value CallInStreamHandlingFunc( 
        std::istream& stream,
        const std::string& name, 
        std::vector<Value>& args,
        Token from, 
        const CallStack& call_stack
    );

    void LoadDefault();

private:
    std::unordered_map<std::string, ValueHandlingFunction> functions_;
    std::unordered_map<std::string, OutStreamHandlingFunction> out_stream_functions_;
    std::unordered_map<std::string, InStreamHandlingFunction> in_stream_functions_;
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
template<typename F> requires std::invocable<F, std::vector<Value>&, Token, const CallStack&>
auto MakeBuiltin(const std::string& name, F fn, size_t arg_num) {
    return [name, fn, arg_num](std::vector<Value>& args,
                         Token from,
                         const CallStack& stack) -> Value {
        if (args.size() != arg_num) {
            throw lang_exceptions::ParametersCountError{from, stack, name, arg_num, args.size()};
        }

        return std::invoke(fn, args, from, stack);
    };
}

template<typename F> requires std::invocable<F, std::ostream&, std::vector<Value>&, Token, const CallStack&>
auto MakeBuiltin(const std::string& name, F fn, size_t arg_num) {
    return [name, fn, arg_num](std::ostream& stream,
                         std::vector<Value>& args,
                         Token from,
                         const CallStack& stack) -> Value {
        if (args.size() != arg_num) {
            throw lang_exceptions::ParametersCountError{from, stack, name, arg_num, args.size()};
        }

        return std::invoke(fn, stream, args, from, stack);
    };
}

template<typename F> requires std::invocable<F, std::istream&, std::vector<Value>&, Token, const CallStack&>
auto MakeBuiltin(const std::string& name, F fn, size_t arg_num) {
    return [name, fn, arg_num](std::istream& stream,
                         std::vector<Value>& args,
                         Token from,
                         const CallStack& stack) -> Value {
        if (args.size() != arg_num) {
            throw lang_exceptions::ParametersCountError{from, stack, name, arg_num, args.size()};
        }

        return std::invoke(fn, stream, args, from, stack);
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
        ThrowArgumentTypeError(std::move(from), call_stack, idx, val.GetType(), GetTypeName<T>());
    }
}
    
} // namespace stdlib
    
} // namespace itmoscript
