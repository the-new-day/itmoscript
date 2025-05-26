#pragma once

#include <string>
#include <memory>
#include <vector>

#include "ast/AST.hpp"

namespace itmoscript {

class Environment;

/**
 * @brief Represents a function value in the language.
 * Two Function instances are equal only if they are the exact same object.
 */
struct FunctionObject {
    FunctionObject(
        std::shared_ptr<std::vector<Identifier>> parameters,
        std::shared_ptr<BlockStatement> body
    ) : parameters(parameters), body(body) {}

    std::shared_ptr<std::vector<Identifier>> parameters;
    std::shared_ptr<BlockStatement> body;

    bool operator==(const FunctionObject& other) const {
        return this == &other;
    }
};
    
} // namespace itmoscript

