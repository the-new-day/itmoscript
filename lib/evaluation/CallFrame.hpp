#pragma once

#include "Environment.hpp"

#include <string>
#include <optional>
#include <cstddef>
#include <vector>

namespace itmoscript {

struct CallFrame {
    std::string function_name;
    Token entry_token;
};

using CallStack = std::vector<CallFrame>;
    
} // namespace itmoscript
