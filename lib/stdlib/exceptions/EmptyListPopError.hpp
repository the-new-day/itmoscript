#pragma once

#include "StdUsageError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class EmptyListPopError : public StdUsageError {
public:
    EmptyListPopError(Token token, const CallStack& call_stack) 
        : StdUsageError(
            token, 
            call_stack, 
            "cannot pop from an empty List"
        ) {}

    std::string error_type() const noexcept override {
        return "EmptyListPopError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
