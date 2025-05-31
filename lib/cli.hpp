#pragma once

#include <string>
#include <vector>
#include <expected>

namespace itmoscript {

namespace cli {

struct Parameters {
    bool need_repl = false;
    bool need_lexer_mode = false;
    bool need_parser_mode = false;
    std::string filename;
    bool need_help = false;
};

std::expected<Parameters, std::string> ParseArguments(const std::vector<std::string>& args);
    
} // namespace cli
    
} // namespace itmoscript
