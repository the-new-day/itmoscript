#include "StdLib.hpp"

#include "exceptions/DuplicateNameError.hpp"
#include "evaluation/exceptions/UndefinedNameError.hpp"

#include "numbers.hpp"

namespace itmoscript {

namespace stdlib {

bool StdLib::Has(const std::string& name) const {
    return functions_.contains(name);
}

void StdLib::Register(const std::string& name, BuiltInFunction func) {
    functions_[name] = std::move(func);
}

Value StdLib::Call(
    const std::string& name, 
    const std::vector<Value>& args,
    Token from, 
    const CallStack& call_stack
) {
    if (!functions_.contains(name)) {
        throw lang_exceptions::UndefinedNameError{from, call_stack};
    }
    return std::invoke(functions_[name], args, from, call_stack);
}

void StdLib::LoadDefault() {
    numbers::RegisterAll(*this);
}

} // namespace stdlib

} // namespace itmoscript
