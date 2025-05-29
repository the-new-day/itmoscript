#pragma once

#include "RuntimeError.hpp"
#include "objects/Value.hpp"

namespace itmoscript {

namespace lang_exceptions {

class IndexOperandTypeError : public RuntimeError {
public:
    IndexOperandTypeError(Token token, const CallStack& call_stack, ValueType type) 
        : RuntimeError(token, call_stack, "invalid operand type for index operator: " + GetTypeName(type)) {}

    std::string error_type() const noexcept override {
        return "IndexOperandTypeError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
