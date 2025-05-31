#include "Interpreter.hpp"

#include <fstream>
#include <stdexcept>
#include <format>

namespace itmoscript {

bool Interpreter::Interpret(std::istream& code, std::istream& read, std::ostream& write) {
    try {
        Lexer lexer{code};
        Parser parser{lexer};
        ast::Program root = parser.ParseProgram();
        Evaluator evaluator;

        evaluator.EnableStandardOperators();
        evaluator.EnableStd();

        evaluator.Evaluate(root, read, write);
    } catch (const lang_exceptions::RuntimeError& e) {
        utils::PrintException(write, e, "Runtime error", lang_exceptions::kErrorDetailsIndent);
        return false;
    } catch (const lang_exceptions::LangException& e) {
        utils::PrintException(write, e, "Error", lang_exceptions::kErrorDetailsIndent);
        return false;
    }

    return true;
}

bool Interpreter::InterpretFromFile(const std::string& filename, std::istream& read, std::ostream& write) {
    std::ifstream file{filename};
    if (!file.good()) {
        throw std::runtime_error{std::format("unable to open file '{}'", filename)};
    }

    return Interpret(file, read, write);
}


void Interpreter::StartRepl(ReplMode mode, std::istream& input, std::ostream& output) {
    REPL repl{mode};
    repl.Start(input, output);
}
    
} // namespace itmoscript
