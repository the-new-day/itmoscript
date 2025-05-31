#include <iostream>

#include "lexer/Lexer.hpp"
#include "repl/REPL.hpp"
#include "parser/Parser.hpp"
#include "evaluation/Evaluator.hpp"
#include "LangException.hpp"

bool Interpret(std::istream& input, std::ostream& output);

namespace itmoscript {

class Interpreter {
public:
    bool Interpret(std::istream& code, std::istream& read, std::ostream& write);
    bool InterpretFromFile(const std::string& filename, std::istream& read, std::ostream& write);

    void StartRepl(ReplMode mode, std::istream& read, std::ostream& write);
};
    
} // namespace itmoscript
