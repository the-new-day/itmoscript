#include <iostream>
#include <vector>

#include <lib/interpreter.hpp>

int main(int argc, char** argv) {
    std::string code = R"(
        incr = function(value)
            // 239 239 end end 42
            return value + 1
        end function // 239 239
    )";

    ItmoScript::Lexer lexer{code};

    std::vector<ItmoScript::Token> tokens;
    
    while (lexer.HasNextToken()) {
        tokens.push_back(lexer.GetNextToken());
    }

    for (const auto& token : tokens) {
        std::cout << static_cast<int>(token.type) << ": " << token.literal << std::endl;
    }

    return 0;
}