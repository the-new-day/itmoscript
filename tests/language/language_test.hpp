#include <lib/Interpreter.hpp>
#include <gtest/gtest.h>

inline bool Interpret(std::istream& input, std::ostream& output) {
    itmoscript::Interpreter interpreter;
    return interpreter.Interpret(input, std::cin, output);
}
