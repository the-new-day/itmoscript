#pragma once

#include "StdUsageError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class DuplicateNameError : public StdUsageError {
public:
    DuplicateNameError(Token token, const CallStack& call_stack, const std::string& name) 
        : StdUsageError(token, call_stack, std::format("name '{}' is already defined in the stdlib", name)) {}

    std::string error_type() const noexcept override {
        return "DuplicateNameError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
