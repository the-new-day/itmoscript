#pragma once

#include "RuntimeError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class DuplicateParameterError : public RuntimeError {
public:
    DuplicateParameterError(Token token, const CallStack& call_stack, const std::string& name) 
        : RuntimeError(token, call_stack, std::format("parameter {} is duplicate", name)) {}

    std::string error_type() const noexcept override {
        return "DuplicateParameterError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
