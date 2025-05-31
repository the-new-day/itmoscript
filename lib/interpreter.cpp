#include "interpreter.hpp"

#include <fstream>
#include <stdexcept>
#include <format>

bool Interpret(std::istream& input, std::ostream& output) {
    return false;
}

namespace itmoscript {

bool Interpreter::Interpret(std::istream& code, std::istream& read, std::ostream& write) {
    Lexer lexer{code};
    Parser parser{lexer};
    ast::Program root = parser.ParseProgram();
    Evaluator evaluator;

    evaluator.EnableStandardOperators();
    evaluator.EnableStd();

    try {
        evaluator.Evaluate(root, read, write);
    } catch (const lang_exceptions::RuntimeError& e) {
        utils::PrintException(write, e, "Runtime error", lang_exceptions::kErrorDetailsIndent);
        return false;
    } catch (const lang_exceptions::LangException& e) {
        utils::PrintException(write, e, "Unhandled error", lang_exceptions::kErrorDetailsIndent);
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
