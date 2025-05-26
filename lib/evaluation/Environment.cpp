#include "Environment.hpp"
#include "Value.hpp"

namespace itmoscript {

bool Environment::Has(const std::string& name) const {
    return storage_.contains(name);
}

const Value& Environment::Get(const std::string& name) const {
    return storage_.at(name);
}

void Environment::Set(const std::string& name, const Value& value) {
    storage_[name] = value;
}

void Environment::Set(const std::string& name, Value&& value) {
    storage_[name] = std::move(value);
}

size_t Environment::size() const {
    return storage_.size();
}

const std::unordered_map<std::string,Value>& Environment::storage() const {
    return storage_;
}
    
} // namespace itmoscript

