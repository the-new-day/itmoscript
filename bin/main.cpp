#include <iostream>
#include <vector>

#include "lib/interpreter.hpp"

int main(int argc, char** argv) {
    std::cout << "ITMOScript super-duper-mega language." << std::endl;
    std::cout << "Interactive mode. Yes." << std::endl;

    ItmoScript::ReplMode repl_mode = ItmoScript::ReplMode::kEval;

    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "--lexer") {
            repl_mode = ItmoScript::ReplMode::kLexer;
        } else if (arg1 == "--parser") {
            repl_mode = ItmoScript::ReplMode::kParser;
        }
    }

    ItmoScript::REPL repl{repl_mode};
    
    try {
        repl.Start(std::cin, std::cout);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory error" << std::endl; // TODO: proper error handling
        return -1;
    } catch (...) {
        std::cerr << "An unknown error occured" << std::endl;
        return -1;
    }

    return 0;
}
