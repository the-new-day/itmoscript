#pragma once

#include "Exception.hpp"

namespace itmoscript {

namespace lang_exceptions {

class ParsingError : public Exception {
public:
    ParsingError(size_t line, size_t col, const std::string& message = "invalid syntax") 
        : Exception(Token{.line = line, .column = col}, message) {}

    std::string error_type() const noexcept override {
        return "ParsingError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
