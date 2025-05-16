#pragma once

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

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

    void EvalLexer(const std::string& line, std::ostream& output);
    void EvalParser(const std::string& line, std::ostream& output);
    void Eval(const std::string& line, std::ostream& output);

    void PrintToken(std::ostream& output, const Token& token);

    void PrintParserErrors(const Parser& parser, std::ostream& output);
    void PrintParserError(const ParserError& error, std::ostream& output);
};
    
} // namespace ItmoScript
