#pragma once

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "evaluation/Evaluator.hpp"

#include <string>
#include <iostream>

namespace ItmoScript {

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

    void EvalLexer(std::ostream& output);
    void EvalParser(std::ostream& output);
    void Eval(std::ostream& output);

    void PrintToken(std::ostream& output, const Token& token);

    void PrintParserErrors(const Parser& parser, std::ostream& output);
    void PrintParserError(const ParserError& error, std::ostream& output, size_t indent);

    void PrintEvaluatorErrors(const Evaluator& evaluator, std::ostream& output);
    void PrintEvaluationError(const EvaluationError& error, std::ostream& output, size_t indent);
};
    
} // namespace ItmoScript
