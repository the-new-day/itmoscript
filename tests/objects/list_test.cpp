#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <format>

#include "lib/objects/List.hpp"

using Value = itmoscript::Value;

TEST(ObjectsListTestSuite, SmallListTest) {
    std::vector<Value> values = {
        1,
        1.5,
        Value{std::make_shared<std::string>("aaa")},
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
        1, 2, 3, 4, 5, 6,
    };

    itmoscript::List list = std::make_shared<itmoscript::ListObject>(values);
    ASSERT_EQ(list->size(), values.size());
    ASSERT_EQ(list->data(), values);

    // <start, end, expected>
    std::vector<std::tuple<size_t, size_t, std::vector<Value>>> slices = {
        {0, 6, {1, 2, 3, 4, 5, 6}},
        {0, 5, {1, 2, 3, 4, 5}},
        {0, 1, {1}},
        {0, 0, {}},
        {1, 3, {2, 3}},
        {4, 5, {5}},
        {4, 100, {5, 6}},
    };

    for (const auto& [start, end, expected] : slices) {
        std::vector<Value> slice = list->GetSlice(start, end);
        ASSERT_EQ(slice, expected) 
            << "start: " << start << "; end: " << end;
    }
}
