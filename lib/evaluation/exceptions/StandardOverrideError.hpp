#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class StandardOverrideError : public RuntimeError {
public:
    StandardOverrideError(Token token, const CallStack& call_stack, const std::string& name) 
        : RuntimeError(
            token, call_stack, 
            std::format("cannot override built-in object '{}'", name)
        ) {}

    std::string error_type() const noexcept override {
        return "StandardOverrideError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
