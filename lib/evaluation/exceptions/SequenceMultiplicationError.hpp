#pragma once

#include "Exception.hpp"
#include "lexer/Token.hpp"

namespace itmoscript {

namespace lang_exceptions {

class SequenceMultiplicationError : public Exception {
public:
    SequenceMultiplicationError(Token token, const std::string& message) 
        : Exception(token, message) {}

    std::string error_type() const noexcept override {
        return "SequenceMultiplicationError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
