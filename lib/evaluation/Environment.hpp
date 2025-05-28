#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <memory>

namespace itmoscript {

class Value;

/**
 * @class Environment
 * @brief Holds the current scope's variable and their values.
 * Also has a link to the parent (outer) scope to be able to search for
 * the identifiers there and change their values.
 */
class Environment {
public:
    /**
     * @brief Constructs the environment object with a link to the parent.
     * For the global scope parent == nullptr.
     */
    explicit Environment(std::shared_ptr<Environment> parent)
        : parent_(std::move(parent)) {}

    Environment(const Environment&) = delete;
    Environment(Environment&&) = default;
    Environment& operator=(const Environment&) = delete;
    Environment& operator=(Environment&&) = default;
    ~Environment() = default;

    /**
     * @brief Checks if name is registered in the current scope or 
     * in any of parent scopes.
     */
    bool Has(const std::string& name) const;

    /**
     * @brief Returns value associated with the given name if it exists in the current scope.
     * Otherwise, goes up in the scope chain and searches there.
     * 
     * The caller must be sure that the name exists, always call Has() before using Get().
     */
    const Value& Get(const std::string& name) const;

    /**
     * @brief Assigns the identifier to the given value.
     * @details If it's registered in the current scope, assigns in the current scope.
     * Otherwise, goes up until it founds the identifier.
     * 
     * If the identifier is not found in the scope chain, registers it in the current scope.
     */
    void Set(const std::string& name, Value value);

    /**
     * @brief Assigns the identifier to the given value in the current scope,
     * not going up in the scope chain.
     */
    void SetInLocal(const std::string& name, Value value);

private:
    std::unordered_map<std::string, Value> storage_;
    std::shared_ptr<Environment> parent_;
};
    
} // namespace itmoscript
