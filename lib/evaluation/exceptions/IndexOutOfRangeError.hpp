#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class IndexOutOfRangeError : public RuntimeError {
public:
    IndexOutOfRangeError(Token token, const CallStack& call_stack, Int index, size_t size) 
        : RuntimeError(
            token, 
            call_stack, 
            std::format("sequence has {} elements, got index: {}", size, index)
        ) {}

    std::string error_type() const noexcept override {
        return "IndexOutOfRangeError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
