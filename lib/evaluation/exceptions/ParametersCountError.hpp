#pragma once

#include "Exception.hpp"

#include <format>
#include <cstddef>

namespace itmoscript {

namespace lang_exceptions {

class ParametersCountError : public RuntimeError {
public:
    ParametersCountError(Token token, const CallStack& call_stack, size_t expected, size_t actual) 
        : RuntimeError(token, call_stack, std::format("expected {} parameters, got {}", expected, actual)) {}

    std::string error_type() const noexcept override {
        return "ParametersCountError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
