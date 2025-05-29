#pragma once

#include "RuntimeError.hpp"

namespace itmoscript {

namespace lang_exceptions {

class UnsupportedTypeError : public RuntimeError {
public:
    UnsupportedTypeError(Token token, const CallStack& call_stack, std::string message) 
        : RuntimeError(token, call_stack, std::move(message)) {}

    std::string error_type() const noexcept override {
        return "UnsupportedTypeError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
