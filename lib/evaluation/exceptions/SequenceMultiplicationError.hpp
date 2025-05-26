#pragma once

#include "RuntimeError.hpp"
#include "lexer/Token.hpp"

namespace itmoscript {

namespace lang_exceptions {

class SequenceMultiplicationError : public RuntimeError {
public:
    SequenceMultiplicationError(Token token, const CallStack& call_stack, const std::string& message) 
        : RuntimeError(token, call_stack, message) {}

    std::string error_type() const noexcept override {
        return "SequenceMultiplicationError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
