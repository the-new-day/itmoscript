#pragma once

#include "RuntimeError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class UncallableObjectCallError : public RuntimeError {
public:
    UncallableObjectCallError(Token token, const CallStack& call_stack, const std::string& object) 
        : RuntimeError(token, call_stack, std::format("uncallable object '{}' was called", object)) {}

    std::string error_type() const noexcept override {
        return "UncallableObjectCallError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
