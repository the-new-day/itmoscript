#pragma once

#include <unordered_map>
#include <string>
#include <optional>

#include "Value.hpp"

namespace itmoscript {

class Environment {
public:
    bool Has(const std::string& name) const {
        return storage_.contains(name);
    }

    const Value& Get(const std::string& name) {
        return storage_.at(name);
    }

    void Set(const std::string& name, const Value& value) {
        storage_.insert({name, value});
    }

    void Set(const std::string& name, Value&& value) {
        storage_.insert({name, std::move(value)});
    }

private:
    std::unordered_map<std::string, Value> storage_;
};
    
} // namespace itmoscript

