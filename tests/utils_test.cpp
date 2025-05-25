#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "lib/utils.hpp"

namespace IU = itmoscript::utils;

template<typename T>
void TestParseNumber(const std::vector<std::pair<std::string, std::expected<T, std::errc>>>& expressions) {
    for (const auto& [input, expected] : expressions) {
        std::expected<T, std::errc> result = IU::ParseNumber<T>(input);

        if (expected.has_value()) {
            ASSERT_TRUE(result.has_value()) << "input = " << input << "; expected = " << expected.value();
            ASSERT_EQ(result.value(), expected.value());
        } else {
            ASSERT_FALSE(result.has_value()) << "input = " << input;
            ASSERT_EQ(result.error(), expected.error());
        }
    }
}

TEST(UtilsTestSuite, ParseNumberIntDecimalTest) {
    std::vector<std::pair<std::string, std::expected<int64_t, std::errc>>> expressions = {
        {"123", 123},
        {"-123", -123},
        {"0", 0},
        {"99999", 99999},
        {"9999999999999999999999999999999999999999999999", std::unexpected<std::errc>{std::errc::result_out_of_range}},
        {"aaa", std::unexpected<std::errc>{std::errc::invalid_argument}},
    };

    TestParseNumber(expressions);
}

TEST(UtilsTestSuite, ParseNumberFloatTest) {
    std::vector<std::pair<std::string, std::expected<double, std::errc>>> expressions = {
        {"123.3", 123.3},
        {"-123.3", -123.3},
        {"0.0", 0.0},
        {"99999", 99999.0},
        {"aaa", std::unexpected<std::errc>{std::errc::invalid_argument}},
    };

    TestParseNumber(expressions);
}

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
        std::optional<std::string> result = IU::MultiplyStr(input, times);
        ASSERT_EQ(result, expected);
    }
}
