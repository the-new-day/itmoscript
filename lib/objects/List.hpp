#pragma once

#include <vector>
#include <utility>
#include <cstddef>
#include <memory>
#include <algorithm>

#include "Value.hpp"

namespace itmoscript {

/**
 * @class ListObject
 * @brief Implementation of the List undelying type.
 * The actual List type is a ReferenceValueType, which means it is
 * a reference to the actual object, which is ListObject.
 */
class ListObject {
public:
    ListObject() = default;

    ListObject(std::vector<Value> values)
        : data_(std::move(values)) {}

    size_t size() const { return data_.size(); }
    const std::vector<Value>& data() const { return data_; }
    bool empty() const { return size() == 0; }

    /**
     * @brief Inserts given value before the given position.
     * ReferenceValueTypes are deep-copied when inserted.
     * 
     * Inserting to an invalid position is an undefined behavior.
     * 
     * So, given the code:
     * @code
     * x = [1, 2]
     * y = [3, 4]
     * insert(x, y)
     * @code
     * x[2] contains copy of the y, not a reference to it.
     */
    void Insert(size_t pos, const Value& value);
    
    /**
     * @brief Inserts given value before the given position.
     * Moves value to the list, for more info see Insert(size_t, const Value&).
     */
    void Insert(size_t pos, Value&& value);

    /**
     * @brief Removes element from the list on the given posision.
     * Removering from an invalid position is an undefined behavior.
     */
    void Remove(size_t pos);

    /**
     * @brief Sorts the List, using Value::operator<.
     */
    void Sort();

    Value& At(size_t index) { return data_.at(index); }
    const Value& At(size_t index) const { return data_.at(index); }

    /**
     * @return Returns copy of the array from index start to index end.
     * 
     * If start > size(), returns an empty array.
     * If end > size(), returns array[start : size() - 1].
     */
    std::vector<Value> GetSlice(size_t start, size_t end) const;
    
    bool operator==(const ListObject& other) const = default;
    bool operator!=(const ListObject& other) const = default;

private:
    std::vector<Value> data_;
};

List CreateList(ListObject val);
    
} // namespace itmoscript
