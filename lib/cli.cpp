#include "cli.hpp"

namespace itmoscript {

namespace cli {

std::expected<Parameters, std::string> ParseArguments(const std::vector<std::string>& args) {
    Parameters config;

    for (const std::string& arg : args) {
        if (arg == "--lexer" || arg == "-l") {
            config.need_lexer_mode = true;
            config.need_parser_mode = false;
        } else if (arg == "--parser" || arg == "-p") {
            config.need_parser_mode = true;
            config.need_lexer_mode = false;
        } else if (!arg.starts_with('-')) {
            config.filename = arg;
        } else {
            return std::unexpected{arg};
        }
    }

    if (config.filename.empty() && !config.need_lexer_mode && !config.need_parser_mode) {
        config.need_repl = true;
    }

    return config;
}
    
} // namespace cli

} // namespace itmoscript
