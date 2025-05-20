#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "lib/utils.hpp"

namespace IU = ItmoScript::Utils;

TEST(UtilsTestSuite, MultiplyStrTest) {
    std::vector<std::tuple<std::string, double, std::optional<std::string>>> expressions = {
        {"hello", 2, "hellohello"},
        {"", 100500, ""},
        {"a", 3, "aaa"},
        {"aa", 2.5, "aaaaa"},
        {"a", 2.5, "aa"},
        {"hello", 0, ""},
        {"", 0, ""},
        
        {"hello", -1, std::nullopt},
        {"", -1, std::nullopt},
    };

    for (const auto& [input, times, expected] : expressions) {
        std::optional<std::string> result = ItmoScript::Utils::MultiplyStr(input, times);
        ASSERT_EQ(result, expected);
    }
}
