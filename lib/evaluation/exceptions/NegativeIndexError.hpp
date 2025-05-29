#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class NegativeIndexError : public RuntimeError {
public:
    NegativeIndexError(Token token, const CallStack& call_stack, int64_t given_index) 
        : RuntimeError(token, call_stack, std::format("got negative index {}", given_index)) {}

    std::string error_type() const noexcept override {
        return "NegativeIndexError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
