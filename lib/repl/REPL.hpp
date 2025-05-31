#pragma once

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "evaluation/Evaluator.hpp"

#include <string>
#include <iostream>
#include <type_traits>

namespace itmoscript {

inline const std::string kReplPrompt = ">>> ";

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
};

} // namespace itmoscript
