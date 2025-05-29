#pragma once

#include "StdUsageError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class SqrtFromNegativeError : public StdUsageError {
public:
    SqrtFromNegativeError(Token token, const CallStack& call_stack, const std::string& value) 
        : StdUsageError(
            token, 
            call_stack, 
            std::format("cannot get sqrt from a negative number {}", value)
        ) {}

    std::string error_type() const noexcept override {
        return "SqrtFromNegativeError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
