#pragma once

#include "RuntimeError.hpp"

namespace itmoscript {

namespace lang_exceptions {

class UnexpectedReturnError : public RuntimeError {
public:
    UnexpectedReturnError(Token token, const CallStack& call_stack) 
        : RuntimeError(token, call_stack, "unexpected 'return'") {}

    std::string error_type() const noexcept override {
        return "UnexpectedReturnError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
