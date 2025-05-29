#include "List.hpp"

namespace itmoscript {

std::vector<Value> ListObject::GetSlice(size_t start, size_t end) const {
    if (start > end || start >= data_.size()) return {};
    
    if (end >= data_.size())
        end = data_.size() - 1;

    return std::vector<Value>(data_.begin() + start, data_.begin() + end + 1);
}

List CreateList(ListObject val) {
    return CreateHeavyValue(val);
}

} // namespace itmoscript

