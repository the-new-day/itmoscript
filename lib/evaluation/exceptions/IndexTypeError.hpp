#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

namespace itmoscript {

namespace lang_exceptions {

class IndexTypeError : public RuntimeError {
public:
    IndexTypeError(Token token, const CallStack& call_stack, ValueType type) 
        : RuntimeError(token, call_stack, "expected Int type for the index operator, got " + GetTypeName(type)) {}

    std::string error_type() const noexcept override {
        return "IndexTypeError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
