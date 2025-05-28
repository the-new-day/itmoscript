#include <iostream>
#include <vector>

#include "lib/interpreter.hpp"

int main(int argc, char** argv) {
    std::cout << "ITMOScript super-duper-mega language." << std::endl;
    std::cout << "Interactive mode. Yes." << std::endl;

    itmoscript::ReplMode repl_mode = itmoscript::ReplMode::kEval;

    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "--lexer") {
            repl_mode = itmoscript::ReplMode::kLexer;
        } else if (arg1 == "--parser") {
            repl_mode = itmoscript::ReplMode::kParser;
        }
    }

    itmoscript::REPL repl{repl_mode};
    
    try {
        repl.Start(std::cin, std::cout);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory error" << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Internal exception occured: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown internal error occured: " << std::endl;
        return -1;
    }

    return 0;
}
