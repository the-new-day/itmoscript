#pragma once

#include <unordered_map>
#include <string>
#include <optional>

namespace itmoscript {

class Value;

class Environment {
public:
    bool Has(const std::string& name) const;
    const Value& Get(const std::string& name) const;
    void Set(const std::string& name, const Value& value);
    void Set(const std::string& name, Value&& value);

    size_t size() const;
    const std::unordered_map<std::string, Value>& storage() const;

private:
    std::unordered_map<std::string, Value> storage_;
};
    
} // namespace itmoscript

