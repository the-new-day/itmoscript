#include <iostream>
#include <vector>

#include "lib/interpreter.hpp"

int main(int argc, char** argv) {
    std::cout << "ITMOScript super-duper-mega language." << std::endl;
    std::cout << "Interactive mode. Yes." << std::endl;

    ItmoScript::REPL repl;
    repl.Start(std::cin, std::cout);

    return 0;
}