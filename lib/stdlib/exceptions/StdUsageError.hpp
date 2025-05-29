#pragma once

#include "evaluation/exceptions/RuntimeError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class StdUsageError : public RuntimeError {
public:
    StdUsageError(Token token, const CallStack& call_stack, const std::string& message) 
        : RuntimeError(token, call_stack, message) {}

    std::string error_type() const noexcept override {
        return "StdUsageError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
