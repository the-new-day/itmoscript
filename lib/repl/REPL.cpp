#include "REPL.hpp"
#include "evaluation/Evaluator.hpp"

#include <format>

namespace itmoscript {

void REPL::Start(std::istream& input, std::ostream& output) {
    while (true) { 
        output << kReplPrompt;

        if (!std::getline(input, current_line_)) break;
        if (current_line_.empty()) continue;

        try {
            if (mode_ == ReplMode::kLexer) {
                EvalLexer(output);
            } else if (mode_ == ReplMode::kParser) {
                EvalParser(output);
            } else if (mode_ == ReplMode::kEval) {
                Eval(output);
            }
        } catch (const lang_exceptions::Exception& e) {
            std::cerr << std::format(
                "Unhandled error on line {}, column {}:\n{}", 
                e.line(), 
                e.column(), 
                *utils::MultiplyStr(" ", lang_exceptions::kErrorDetailsIndent)
            );
            std::cerr << std::format("{}: {}", e.error_type(), e.what()) << std::endl;

            std::cout << current_line_ << std::endl;
            std::cout << *utils::MultiplyStr(" ", e.column());
            std::cout << '^' << std::endl;
        }
    }
}

void REPL::EvalLexer(std::ostream& output) {
    Lexer lexer{current_line_};
    Token token = lexer.GetNextToken();

    while (token.type != TokenType::kEOF) {
        PrintToken(output, token);
        output << std::endl;
        token = lexer.GetNextToken();
    }
}

void REPL::EvalParser(std::ostream& output) {
    Lexer lexer{current_line_};
    Parser parser{lexer};
    Program program = parser.ParseProgram();

    output << program.String();
    output << '\n';
}

void REPL::Eval(std::ostream& output) {
    Lexer lexer{current_line_};
    Parser parser{lexer};
    Program program = parser.ParseProgram();

    evaluator_.Interpret(program);

    output << evaluator_.GetLastEvaluatedValue();
    output << '\n';
}

void REPL::PrintToken(std::ostream& output, const Token& token) {
    output << std::format(
        "Ln {}, Col {} [{}({})]",
        token.line,
        token.column,
        kTokenTypeNames.at(token.type),
        token.literal
    );
}

} // namespace itmoscript
