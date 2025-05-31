#pragma once

#include <string>
#include <optional>
#include <cstddef>
#include <vector>

namespace itmoscript {

/**
 * @struct CallFrame
 * @brief Represents the current call frame, used for stacktrace printing
 * and debugging. Contains the function name and the entry token (position of the call).
 */
struct CallFrame {
    std::string function_name;
    Token entry_token;
};

using CallStack = std::vector<CallFrame>;
    
} // namespace itmoscript
