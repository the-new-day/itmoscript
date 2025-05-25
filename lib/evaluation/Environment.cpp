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
    storage_.insert({name, value});
}

void Environment::Set(const std::string& name, Value&& value) {
    storage_.insert({name, std::move(value)});
}
    
} // namespace itmoscript

