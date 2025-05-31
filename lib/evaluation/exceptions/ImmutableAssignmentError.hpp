#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class ImmutableAssignmentError : public RuntimeError {
public:
    ImmutableAssignmentError(
        Token token,
        const CallStack& call_stack,
        const std::string& name,
        ValueType type) 
        : RuntimeError(
            token, 
            call_stack, 
            std::format(
                "cannot assign to an immutable object '{}' of type {}",
                name, GetTypeName(type)
            )
        ) {}

    std::string error_type() const noexcept override {
        return "ImmutableAssignmentError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
