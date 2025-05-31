#pragma once

#include "StdUsageError.hpp"

#include <format>

namespace itmoscript {

namespace lang_exceptions {

class FileAccessError : public StdUsageError {
public:
    FileAccessError(Token token, const CallStack& call_stack, const std::string& filename) 
        : StdUsageError(
            token, 
            call_stack, 
            std::format("unable to access file '{}'", filename)
        ) {}

    std::string error_type() const noexcept override {
        return "FileAccessError";
    }
};

} // namespace lang_exceptions
    
} // namespace itmoscript
