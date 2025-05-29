#pragma once

#include "StdUsageError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class InvalidArgumentError : public StdUsageError {
public:
    InvalidArgumentError(
        Token token, 
        const CallStack& call_stack,
        size_t idx,
        const std::string& message) 
        : StdUsageError(
            token, 
            call_stack, 
            std::format(
                "invalid argument on index {}: {}",
                idx,
                message
            )
        ) {}

    std::string error_type() const noexcept override {
        return "InvalidArgumentError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
