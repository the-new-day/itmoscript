#include "Environment.hpp"
#include "objects/Value.hpp"

namespace itmoscript {

bool Environment::Has(const std::string& name) const {
    return storage_.contains(name) || (parent_ != nullptr && parent_->Has(name));
}

const Value& Environment::Get(const std::string& name) const {
    if (storage_.contains(name)) {
        return storage_.at(name);
    } else {
        return parent_->Get(name);
    }
}

void Environment::Set(const std::string& name, Value value) {
    if (storage_.contains(name)) {
        storage_[name] = std::move(value);
    } else if (parent_ != nullptr && parent_->Has(name)) {
        parent_->Set(name, std::move(value));
    } else {
        storage_[name] = std::move(value);
    }
}

void Environment::SetInLocal(const std::string& name, Value value) {
    storage_[name] = std::move(value);
}

} // namespace itmoscript
