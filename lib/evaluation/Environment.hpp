#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <memory>

namespace itmoscript {

class Value;

class Environment {
public:
    explicit Environment(std::shared_ptr<Environment> parent)
        : parent_(std::move(parent)) {}

    Environment(const Environment&) = delete;
    Environment(Environment&&) = default;
    Environment& operator=(const Environment&) = delete;
    Environment& operator=(Environment&&) = default;
    ~Environment() = default;

    bool Has(const std::string& name) const;
    const Value& Get(const std::string& name) const;
    void Set(const std::string& name, Value value);

private:
    std::unordered_map<std::string, Value> storage_;
    std::shared_ptr<Environment> parent_;
};
    
} // namespace itmoscript
