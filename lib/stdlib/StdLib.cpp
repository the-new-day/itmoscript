#include "StdLib.hpp"

#include "exceptions/DuplicateNameError.hpp"
#include "evaluation/exceptions/UndefinedNameError.hpp"

#include "numbers.hpp"
#include "strings.hpp"
#include "lists.hpp"
#include "sys.cpp"

namespace itmoscript {

namespace stdlib {

bool StdLib::Has(const std::string& name) const {
    return HasValueHandlingFunc(name)
        || HasOutStreamHandlingFunc(name)
        || HasInStreamHandlingFunc(name);
}

bool StdLib::HasOutStreamHandlingFunc(const std::string& name) const {
    return out_stream_functions_.contains(name);
}

bool StdLib::HasInStreamHandlingFunc(const std::string& name) const {
    return in_stream_functions_.contains(name);
}

bool StdLib::HasValueHandlingFunc(const std::string& name) const {
    return functions_.contains(name);
}

void StdLib::Register(const std::string& name, ValueHandlingFunction func) {
    functions_[name] = std::move(func);
}

void StdLib::RegisterOutStreamHandlingFunc(const std::string& name, OutStreamHandlingFunction func) {
    out_stream_functions_[name] = std::move(func);
}

void StdLib::RegisterInStreamHandlingFunc(const std::string& name, InStreamHandlingFunction func) {
    in_stream_functions_[name] = std::move(func);
}

Value StdLib::Call(
    const std::string& name, 
    std::vector<Value>& args,
    Token from, 
    const CallStack& call_stack
) {
    if (!functions_.contains(name)) {
        throw lang_exceptions::UndefinedNameError{from, call_stack};
    }
    return std::invoke(functions_[name], args, from, call_stack);
}

Value StdLib::CallOutStreamHandlingFunc(
    std::ostream& stream,
    const std::string& name, 
    std::vector<Value>& args, 
    Token from, 
    const CallStack& call_stack
) {
    if (!out_stream_functions_.contains(name)) {
        throw lang_exceptions::UndefinedNameError{from, call_stack};
    }
    return std::invoke(out_stream_functions_[name], stream, args, from, call_stack);
}

Value StdLib::CallInStreamHandlingFunc(
    std::istream& stream, 
    const std::string & name, 
    std::vector<Value>& args, 
    Token from, 
    const CallStack & call_stack
) {
    if (!in_stream_functions_.contains(name)) {
        throw lang_exceptions::UndefinedNameError{from, call_stack};
    }
    return std::invoke(in_stream_functions_[name], stream, args, from, call_stack);
}

void StdLib::LoadDefault() {
    numbers::RegisterAll(*this);
    strings::RegisterAll(*this);
    lists::RegisterAll(*this);
    sys::RegisterAll(*this);
}

void ThrowArgumentTypeError(
    Token from, 
    const CallStack &call_stack, 
    size_t idx, 
    ValueType given_type, 
    const std::string &expected
) {
    ThrowError<lang_exceptions::ArgumentTypeError>(std::move(from), call_stack, idx, given_type, expected);
}

} // namespace stdlib

} // namespace itmoscript
