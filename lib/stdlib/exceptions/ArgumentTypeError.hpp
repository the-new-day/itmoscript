#pragma once

#include "StdUsageError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class ArgumentTypeError : public StdUsageError {
public:
    ArgumentTypeError(
        Token token, 
        const CallStack& call_stack,
        size_t idx,
        ValueType given,
        const std::string& expected) 
        : StdUsageError(
            token, 
            call_stack, 
            std::format(
                "invalid argument type on index {}: expected {}, got {}",
                idx,
                expected,
                GetTypeName(given)
            )
        ) {}

    std::string error_type() const noexcept override {
        return "ArgumentTypeError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
