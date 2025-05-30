#include "List.hpp"

namespace itmoscript {

void ListObject::Insert(size_t pos, const Value& value) {
    if (value.IsReferenceType()) {
        data_.insert(data_.begin() + pos, value.GetCopy());
    } else {
        data_.insert(data_.begin() + pos, value);
    }
}

void ListObject::Insert(size_t pos, Value&& value) {
    data_.insert(data_.begin() + pos, std::move(value));
}

void ListObject::Remove(size_t pos) {
    data_.erase(data_.begin() + pos);
}

void ListObject::Sort() {
    std::sort(data_.begin(), data_.end());
}

std::vector<Value> ListObject::GetSlice(size_t start, size_t end) const {
    if (start > end || start >= data_.size()) return {};
    
    if (end > data_.size())
        end = data_.size();

    return std::vector<Value>(data_.begin() + start, data_.begin() + end);
}

List CreateList(ListObject val) {
    return CreateRefValue(val);
}

} // namespace itmoscript

