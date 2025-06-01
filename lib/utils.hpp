#pragma once

#include <optional>
#include <expected>
#include <string_view>
#include <charconv>
#include <cstdint>
#include <string>
#include <type_traits>
#include <concepts>
#include <sstream>
#include <functional>

namespace itmoscript {

namespace utils {

int64_t FastPow(int64_t base, uint64_t exponent);
double FastPowNeg(int64_t base, int64_t exponent);
void ReplaceAll(std::string& str, const std::string& from, const std::string& to);
std::optional<std::string> MultiplyStr(const std::string& str, double times);

template<std::integral T>
std::expected<T, std::errc> ParseNumber(std::string_view str, int base = 10) {
    T result;
    std::from_chars_result convertion_result = std::from_chars(str.data(), str.data() + str.size(), result, base);

    if (convertion_result.ptr != str.end() || convertion_result.ec == std::errc::result_out_of_range) {
        return std::unexpected{convertion_result.ec};
    } else {
        return result;
    }
}

template<std::floating_point T>
std::expected<T, std::errc> ParseNumber(std::string_view str) {
    T result;
    std::from_chars_result convertion_result = std::from_chars(str.data(), str.data() + str.size(), result);

    if (convertion_result.ptr != str.end() || convertion_result.ec == std::errc::result_out_of_range) {
        return std::unexpected{convertion_result.ec};
    } else {
        return result;
    }
}

template<typename T>
std::optional<std::vector<T>> MultiplyVec(const std::vector<T>& vec, double times) {
    if (times < 0) return std::nullopt;

    size_t whole_part = static_cast<size_t>(times);
    size_t additional_chars = static_cast<double>(vec.size()) * (times - static_cast<double>(whole_part));

    std::vector<T> result;
    result.reserve(vec.size() * whole_part + additional_chars);

    for (size_t i = 0; i < whole_part; ++i) {
        for (size_t j = 0; j < vec.size(); ++j) {
            result.push_back(vec[j]);
        }
    }

    for (size_t i = 0; i < additional_chars; ++i)
        result.push_back(vec[i]);

    return result;
}

template<typename T, typename Out = T>
std::string Join(const std::vector<T>& objects, const std::string& glue, const std::function<Out(const T&)>& getter) {
    std::ostringstream result;
    for (size_t i = 0; i < objects.size(); ++i) {
        result << std::invoke(getter, objects[i]);
        if (i != objects.size() - 1) result << glue;
    }
    return result.str();
}

template<typename T>
void PrintException(std::ostream& output, const T& e, const std::string& label) {
    output << std::format(
        "{} on line {}, column {}:\n",
        label,
        e.line(),
        e.column()
    );

    output << std::format("{}: {}", e.error_type(), e.what()) << std::endl;
}

} // namespace utils

} // namespace itmoscript
