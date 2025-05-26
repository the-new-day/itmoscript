#pragma once

#include "RuntimeError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class UndefinedNameError : public RuntimeError {
public:
    UndefinedNameError(Token token, const CallStack& call_stack) 
        : RuntimeError(token, call_stack, std::format("name '{}' is not defined", token.literal)) {}

    std::string error_type() const noexcept override {
        return "UndefinedNameError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
