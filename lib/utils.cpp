#include "utils.hpp"

namespace itmoscript {

namespace utils {

int64_t FastPow(int64_t base, uint64_t exponent) {
    int64_t result = 1;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result *= base;
        }

        base *= base;
        exponent /= 2;
    }

    return result;
}

double FastPowNeg(int64_t base, int64_t exponent) {
    uint64_t abs_exponent = (exponent < 0) ? -exponent : exponent;

    int64_t result = 1;
    while (abs_exponent > 0) {
        if (abs_exponent % 2 == 1) {
            result *= base;
        }

        base *= base;
        abs_exponent /= 2;
    }

    return (exponent < 0) ? (1.0 / result) : result;
}

std::optional<std::string> MultiplyStr(const std::string& str, double times) {
    if (times < 0) return std::nullopt;

    size_t whole_part = static_cast<size_t>(times);
    size_t additional_chars = static_cast<double>(str.size()) * (times - static_cast<double>(whole_part));

    std::string result;
    result.reserve(str.size() * whole_part + additional_chars);

    for (size_t i = 0; i < whole_part; ++i) {
        for (size_t j = 0; j < str.size(); ++j) {
            result.push_back(str[j]);
        }
    }

    for (size_t i = 0; i < additional_chars; ++i)
        result.push_back(str[i]);

    return result;
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return;
    }

    std::string new_str;
    new_str.reserve(str.size() + (to.size() > from.size() ? (to.size() - from.size()) * 10 : 0));

    size_t start_pos = 0;
    size_t find_pos = 0;

    while ((find_pos = str.find(from, find_pos)) != std::string::npos) {
        new_str.append(str, start_pos, find_pos - start_pos);
        new_str += to;
        find_pos += from.length();
        start_pos = find_pos;
    }

    new_str.append(str, start_pos);
    new_str.swap(str);
}

} // namespace utils

} // namespace itmoscript
