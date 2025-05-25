#pragma once

#include "Exception.hpp"
#include "lexer/Token.hpp"

namespace itmoscript {

namespace lang_exceptions {

class ZeroDivisionError : public Exception {
public:
    ZeroDivisionError(Token token) 
        : Exception(token, "zero division error") {}

    std::string error_type() const noexcept override {
        return "ZeroDivisionError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
