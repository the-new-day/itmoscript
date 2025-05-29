#pragma once

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "evaluation/Evaluator.hpp"

#include <string>
#include <iostream>
#include <type_traits>

namespace itmoscript {

const std::string kReplPrompt = ">>> ";

enum class ReplMode {
    kLexer,
    kParser,
    kEval
};

class REPL {
public:
    REPL(ReplMode mode = ReplMode::kEval)
        : mode_(mode) {}

    void Start(std::istream& input, std::ostream& output);

private:
    ReplMode mode_;
    std::string current_line_;

    Evaluator evaluator_;

    void EvalLexer(std::ostream& output);
    void EvalParser(std::ostream& output);
    void Eval(std::istream& input, std::ostream& output);

    void PrintToken(std::ostream& output, const Token& token);

    template<typename T>
    void PrintException(std::ostream& output, const T& e, const std::string& error_type);
};
    
template<typename T>
void REPL::PrintException(std::ostream& output, const T& e, const std::string& label) {
    if constexpr (std::is_same_v<T, lang_exceptions::RuntimeError>) {
        output << e.GetCallStackMessage() << std::endl;
    }

    output << std::format(
        "{} on line {}, column {}:\n",
        label,
        e.line(),
        e.column(),
        *utils::MultiplyStr(" ", lang_exceptions::kErrorDetailsIndent)
    );

    output << std::format("{}: {}", e.error_type(), e.what()) << std::endl;
}

} // namespace itmoscript
