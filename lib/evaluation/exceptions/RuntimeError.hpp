#pragma once

#include "Exception.hpp"
#include "evaluation/CallFrame.hpp"
#include "utils.hpp"

#include <format>
#include <vector>

namespace itmoscript {

namespace lang_exceptions {

class RuntimeError : public Exception {
public:
    RuntimeError(Token token, const CallStack& call_stack, const std::string& message)
        : Exception(std::move(token), message) 
    {
        call_stack_ = call_stack;
    }

    std::string error_type() const noexcept override {
        return "RuntimeError";
    }

    std::string GetCallStackMessage() const {
        std::string result = "Traceback (most recent call last):\n";

        for (const CallFrame& frame : call_stack_) {
            result += *utils::MultiplyStr(" ", kErrorDetailsIndent);
            result += frame.function_name;
            result += std::format(", on line {}", frame.entry_token.line);
            result += '\n';
        }

        return result;
    }

    const CallStack& call_stack() const {
        return call_stack_;
    }

private:
    std::string stacktrace_message_;
    CallStack call_stack_;
};
    
} // namespace lang_exceptions

} // namespace itmoscript
