#include "numbers.hpp"
#include "StdLib.hpp"
#include "utils.hpp"

#include "exceptions/SqrtFromNegativeError.hpp"
#include "exceptions/InvalidArgumentError.hpp"

#include <cmath>
#include <random>
#include <string>

namespace itmoscript {

namespace stdlib {

namespace numbers {

void RegisterAll(StdLib& lib) {
    lib.Register("abs", MakeBuiltin(Abs, 1));
    lib.Register("ceil", MakeBuiltin(Ceil, 1));
    lib.Register("floor", MakeBuiltin(Floor, 1));
    lib.Register("round", MakeBuiltin(Round, 1));
    lib.Register("sqrt", MakeBuiltin(Sqrt, 1));
    lib.Register("rnd", MakeBuiltin(Rnd, 1));
    lib.Register("parse_num", MakeBuiltin(ParseNum, 1));
    lib.Register("to_string", MakeBuiltin(ToString, 1));
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
) {
    ThrowError<lang_exceptions::ArgumentTypeError>(std::move(from), call_stack, idx, given_type, expected);
}

void AssertIntOrFloat(const Value& val, size_t idx, Token from, const CallStack& call_stack) {
    if (!val.IsOfType<Int>() && !val.IsOfType<Float>()) {
        ThrowArgumentTypeError(std::move(from), call_stack, idx, val.type(), "Int or Float");
    }
}

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

Value Abs(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertIntOrFloat(args[0], 0, from, call_stack);

    if (args[0].IsOfType<Int>()) {
        return std::abs(args[0].Get<Int>());
    }

    return std::abs(args[0].Get<Float>());
}

Value Ceil(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertIntOrFloat(args[0], 0, from, call_stack);
    if (args[0].IsOfType<Int>()) {
        return args[0].Get<Int>();
    }

    return std::ceil(args[0].Get<Float>());
}

Value Floor(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertIntOrFloat(args[0], 0, from, call_stack);
    if (args[0].IsOfType<Int>()) {
        return args[0].Get<Int>();
    }

    return std::floor(args[0].Get<Float>());
}

Value Round(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertIntOrFloat(args[0], 0, from, call_stack);
    if (args[0].IsOfType<Int>()) {
        return args[0].Get<Int>();
    }

    return std::round(args[0].Get<Float>());
}

Value Sqrt(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertIntOrFloat(args[0], 0, from, call_stack);
    if (args[0].IsOfType<Int>()) {
        if (args[0].Get<Int>() < 0) {
            ThrowError<lang_exceptions::SqrtFromNegativeError>(from, call_stack, args[0].ToString());
        }

        return std::sqrt(args[0].Get<Int>());
    }

    if (args[0].Get<Float>() < 0) {
        ThrowError<lang_exceptions::SqrtFromNegativeError>(from, call_stack, args[0].ToString());
    }

    return std::sqrt(args[0].Get<Float>());
}

Value Rnd(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<Int>(args[0], 0, from, call_stack);
    Int n = args[0].Get<Int>();

    if (n <= 0) {
        ThrowError<lang_exceptions::InvalidArgumentError>(
            from, call_stack, 0,
            std::format("negative values and zero are not supported, got {}", n)
        );
    }

    std::random_device rdev;
    std::mt19937 rgen{rdev()};
    std::uniform_int_distribution<int64_t> idist(0, n - 1);

    return idist(rgen);
}

Value ParseNum(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);
    auto try_float = utils::ParseNumber<Float>(*args[0].Get<String>());

    if (try_float.has_value()) {
        if (static_cast<Int>(*try_float) == try_float) {
            return static_cast<Int>(*try_float);
        }
        return *try_float;
    }

    auto try_int = utils::ParseNumber<Int>(*args[0].Get<String>());

    if (try_int.has_value()) {
        return *try_int;
    }

    return NullType{};
}

Value ToString(const std::vector<Value> &args, Token from, const CallStack& call_stack) {
    AssertIntOrFloat(args[0], 0, from, call_stack);
    if (args[0].IsOfType<Int>()) {
        return CreateString(std::to_string(args[0].Get<Int>()));
    }
    
    return CreateString(std::to_string(args[0].Get<Float>()));
}

} // namespace math

} // namespace stdlib

} // namespace itmoscript
