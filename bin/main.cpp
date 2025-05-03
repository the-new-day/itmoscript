#include <iostream>
#include <vector>

#include <lib/interpreter.hpp>

int main(int argc, char** argv) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function
    )";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> tokens;
    
    while (lexer.HasNextToken()) {
        tokens.push_back(lexer.GetNextToken());
    }

    for (const auto& [type, literal] : tokens) {
        std::cout << literal << std::endl;
    }

    return 0;
}