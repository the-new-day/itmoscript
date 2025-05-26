#pragma once

#include <stdexcept>
#include <string>
#include <format>

#include "lexer/Token.hpp"

namespace itmoscript {

namespace lang_exceptions {

/** 
 * @brief Error details indent for printing.
 * The error is printed as:
 * 
 * <ErrorType>:
 * 
 * (' ' * kErrorDetailsIndent)<message>
 */
const size_t kErrorDetailsIndent = 4;

/**
 * @brief Generic exception type for all kinds of errors.
 * All specific exception types should be inherited from it.
 */
class Exception : public std::exception {
public:
    /**
     * @brief Constructs an expression with the token details.
     * Token is used to generate a message including error position details.
     */
    Exception(Token token, std::string message)
        : message_(std::move(message))
        , token_(std::move(token)) {}

    /** @brief Returns the printed error type. All inheritants of Exception should implement this. */
    virtual std::string error_type() const noexcept {
        return "Exception";
    }

    const char* what() const noexcept override {
        return message_.c_str();
    }

    /** @brief Returns current token's line (on which the error occured) */
    size_t line() const {
        return token_.line;
    }

    /** @brief Returns current token's column in the line (on which the error occured) */
    size_t column() const {
        return token_.column;
    }

    virtual ~Exception() = default;

protected:
    Token token_;
    std::string message_;
};

} // namespace exceptions
    
} // namespace itmoscript
