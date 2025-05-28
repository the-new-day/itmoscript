#pragma once

#include <string>
#include <memory>
#include <vector>

#include "ast/AST.hpp"

namespace itmoscript {

/**
 * @brief Represents a function value in the language.
 * Two Function instances are equal only if they are the exact same object.
 */
struct FunctionObject {
    FunctionObject(
        std::shared_ptr<std::vector<std::shared_ptr<ast::Identifier>>> parameters,
        std::shared_ptr<ast::BlockStatement> body
    ) : parameters(std::move(parameters)), body(std::move(body)) {}

    std::shared_ptr<std::vector<std::shared_ptr<ast::Identifier>>> parameters;
    std::shared_ptr<ast::BlockStatement> body;

    bool operator==(const FunctionObject& other) const {
        return this == &other;
    }
};

class Function {
public:
    explicit Function(std::shared_ptr<FunctionObject> obj_) 
      : obj(std::move(obj_)) {}
      
    explicit Function(
        std::shared_ptr<std::vector<std::shared_ptr<ast::Identifier>>> parameters, 
        std::shared_ptr<ast::BlockStatement> body) 
      : obj(std::make_shared<FunctionObject>(std::move(parameters), std::move(body))) {}

    bool operator==(const Function& other) const {
        return obj == other.obj;
    }

    bool operator!=(const Function& other) const {
        return !(*this == other);
    }

    const std::vector<std::shared_ptr<ast::Identifier>>& parameters() const {
        return *obj->parameters;
    }

    std::shared_ptr<ast::BlockStatement> body() const {
        return obj->body;
    }

private:
    std::shared_ptr<FunctionObject> obj;
};

} // namespace itmoscript
