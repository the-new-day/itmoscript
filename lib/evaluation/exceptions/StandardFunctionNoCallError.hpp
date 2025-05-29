#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class StandardFunctionNoCallError : public RuntimeError {
public:
    StandardFunctionNoCallError(Token token, const CallStack& call_stack) 
        : RuntimeError(
            token, call_stack, 
            std::format("built-in function {} used in a non-call expression", token.literal)
        ) {}

    std::string error_type() const noexcept override {
        return "StandardFunctionNoCallError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
