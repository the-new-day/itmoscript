#pragma once

#include "RuntimeError.hpp"
#include "lexer/Token.hpp"

namespace itmoscript {

namespace lang_exceptions {

class ZeroDivisionError : public RuntimeError {
public:
    ZeroDivisionError(Token token, const CallStack& call_stack) 
        : RuntimeError(token, call_stack, "zero division error") {}

    std::string error_type() const noexcept override {
        return "ZeroDivisionError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
