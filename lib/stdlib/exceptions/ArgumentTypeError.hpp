#pragma once

#include "StdUsageError.hpp"

namespace itmoscript {

namespace lang_exceptions {

class ArgumentTypeError : public StdUsageError {
public:
    ArgumentTypeError(
        Token token, 
        const CallStack& call_stack,
        size_t idx,
        ValueType given,
        ValueType expected) 
        : StdUsageError(
            token, 
            call_stack, 
            std::format(
                "invalid argument on index {}: expected {}, got {}",
                idx,
                GetTypeName(expected),
                GetTypeName(given)
            )
        ) {}

    std::string error_type() const noexcept override {
        return "ArgumentTypeError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
