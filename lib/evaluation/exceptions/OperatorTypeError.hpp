#pragma once

#include <format>

#include "RuntimeError.hpp"
#include "evaluation/Value.hpp"

namespace itmoscript {

namespace lang_exceptions {

/** 
 * @brief Error that occured when a user tried to use an opertor on unsupported types. 
 * 
 * For example, if something like 5 / "hello" is written.
 */
class OperatorTypeError : public RuntimeError {
public:
    OperatorTypeError(
        Token token, 
        const CallStack& call_stack, 
        const std::string& oper, 
        ValueType left, 
        ValueType right, 
        const std::string& message = "") 
        : RuntimeError(
            token,
            call_stack,
            std::format(
                "unsupported operand types for '{}': {} and {}{}",
                oper,
                kValueTypeNames.at(left),
                kValueTypeNames.at(right),
                message.empty() ? "" : ", " + message
            )
        ) {}

    OperatorTypeError(
        Token token, 
        const CallStack& call_stack, 
        const std::string& oper, 
        ValueType right, 
        const std::string& message = "") 
        : RuntimeError(
            token,
            call_stack,
            std::format(
                "unsupported operand type for '{}': {}{}",
                oper,
                kValueTypeNames.at(right),
                message.empty() ? "" : ", " + message
            )
        ) {}

    std::string error_type() const noexcept override {
        return "TypeError";
    }
};
    
} // namespace lang_exceptions
    
} // namespace itmoscript
