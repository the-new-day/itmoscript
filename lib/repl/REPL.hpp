#pragma once

#include "lexer/Lexer.hpp"
#include <string>
#include <iostream>

namespace ItmoScript {

const std::string kReplPrompt = ">> ";

class REPL {
public:

    void Start(std::istream& input, std::ostream& output);

private:
    void PrintToken(std::ostream& output, const Token& token);
};
    
} // namespace ItmoScript

