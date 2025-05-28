#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <format>

#include "lib/objects/List.hpp"

using Value = itmoscript::Value;

std::vector<Value> GetSlice(std::vector<Value> data, size_t start, size_t end) {
    if (start < end) return {};
    
    if (end >= data.size())
        end = data.size() - 1;

    std::vector<Value> slice;

    for (size_t i = start; i <= end; ++i) {
        slice.push_back(data[i]);
    }

    return slice;
}

TEST(ObjectsListTestSuite, SmallListTest) {
    std::vector<Value> values = {
        Value{1},
        Value{1.5},
        Value{std::string{"aaa"}},
        Value{true}
    };

    itmoscript::List list = std::make_shared<itmoscript::ListObject>(values);
    ASSERT_EQ(list->size(), values.size());
    ASSERT_EQ(list->data(), values);
}

TEST(ObjectsListTestSuite, EmptyListTest) {
    std::vector<Value> values = {};

    itmoscript::List list = std::make_shared<itmoscript::ListObject>(values);
    ASSERT_EQ(list->size(), values.size());
    ASSERT_EQ(list->data(), values);
}

TEST(ObjectsListTestSuite, ListSliceTest) {
    std::vector<Value> values = {
        Value{1},
        Value{2},
        Value{3},
        Value{4},
        Value{5},
        Value{6},
    };

    itmoscript::List list = std::make_shared<itmoscript::ListObject>(values);
    ASSERT_EQ(list->size(), values.size());
    ASSERT_EQ(list->data(), values);

    // <start, end>
    std::vector<std::pair<size_t, size_t>> slices = {
        {0, 5},
        {0, 1},
        {0, 0},
        {1, 3},
        {4, 5},
        {4, 100},
    };

    for (const auto& [start, end] : slices) {
        std::vector<Value> slice = list->GetSlice(start, end);
        ASSERT_EQ(slice, GetSlice(values, start, end)) << "start: " << start << "; end: " << end;
    }
}
