#pragma once

#include "Exception.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class NameError : public Exception {
public:
    NameError(Token token) 
        : Exception(token, std::format("name '{}' is not defined", token.literal)) {}

    std::string error_type() const noexcept override {
        return "NameError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
