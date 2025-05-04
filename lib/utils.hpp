#pragma once

#include <optional>
#include <string_view>
#include <charconv>

namespace ItmoScript {

template<typename T>
std::optional<T> ParseNumber(std::string_view str) {
    T result;
    std::from_chars_result convertion_result = std::from_chars(str.data(), str.data() + str.size(), result);

    if (convertion_result.ec == std::errc::invalid_argument || convertion_result.ptr != str.end()) {
        return std::nullopt;
    } else if (convertion_result.ec == std::errc::result_out_of_range) {
        return std::nullopt;
    }

    return result;
}
    
} // namespace ItmoScript

