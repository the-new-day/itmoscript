#pragma once

#include <vector>
#include <utility>
#include <cstddef>
#include <memory>
#include <algorithm>

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

    void Push(const Value& value) { data_.push_back(value); }
    void Append(Value&& value) { data_.push_back(std::move(value)); }

    void Pop() { data_.pop_back(); }

    void Insert(size_t pos, const Value& value) { data_.insert(data_.begin() + pos, value); }
    void Insert(size_t pos, Value&& value) { data_.insert(data_.begin() + pos, std::move(value)); }

    void Remove(size_t pos) { data_.erase(data_.begin() + pos); }

    void Sort() { std::sort(data_.begin(), data_.end()); }

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

List CreateList(ListObject val);
    
} // namespace itmoscript
