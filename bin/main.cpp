#include <iostream>
#include <vector>

#include "lib/Interpreter.hpp"
#include "lib/cli.hpp"

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::expected params = itmoscript::cli::ParseArguments(args);

    if (!params.has_value()) {
        std::cerr << "Unknown argument: " << params.error() << std::endl;
        return -1;
    }
    
    try {
        itmoscript::Interpreter interpreter;

        if (params->need_repl || params->filename.empty()) {
            std::cout << "ITMOScript super-duper-mega language." << std::endl;
            std::cout << "Interactive mode. Yes." << std::endl;

            itmoscript::ReplMode repl_mode = itmoscript::ReplMode::kEval;
            if (params->need_lexer_mode) {
                repl_mode = itmoscript::ReplMode::kLexer;
            } else if (params->need_parser_mode) {
                repl_mode = itmoscript::ReplMode::kParser;
            }

            interpreter.StartRepl(repl_mode, std::cin, std::cout);
            return 0;
        }

        if (params->need_lexer_mode || params->need_parser_mode) {
            std::cerr << "Execution mode (--lexer or --parser) is only available in REPL" << std::endl;
            return -1;
        }

        if (!params->filename.empty()) {
            interpreter.InterpretFromFile(params->filename, std::cin, std::cout);
            return 0;
        }
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory error" << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Error occured: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown internal error occured: " << std::endl;
        return -1;
    }

    return 0;
}
