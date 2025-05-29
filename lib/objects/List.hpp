#pragma once

#include <vector>
#include <utility>
#include <cstddef>
#include <memory>

#include "Value.hpp"

namespace itmoscript {

class ListObject {
public:
    ListObject() = default;

    ListObject(std::vector<Value> values)
        : data_(std::move(values)) {}

    size_t size() const { return data_.size(); }
    const std::vector<Value>& data() const { return data_; }
    bool empty() const { return size() == 0; }

    std::vector<Value> GetSlice(size_t start, size_t end) const;
    
    bool operator==(const ListObject& other) const {
        return data_ == other.data_;
    }

    bool operator!=(const ListObject& other) const {
        return !(*this == other);
    }

private:
    std::vector<Value> data_;
};
    
} // namespace itmoscript
