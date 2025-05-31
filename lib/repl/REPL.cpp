#include "REPL.hpp"
#include "evaluation/Evaluator.hpp"

#include <format>

namespace itmoscript {

void REPL::Start(std::istream& input, std::ostream& output) {
    if (mode_ == ReplMode::kEval) {
        evaluator_.EnableStandardOperators();
        evaluator_.EnableStd();
    }

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
                Eval(input, output);
            }
        } catch (const lang_exceptions::RuntimeError& e) {
            utils::PrintException(output, e, "Runtime error", lang_exceptions::kErrorDetailsIndent);
        } catch (const lang_exceptions::LangException& e) {
            utils::PrintException(output, e, "Error", lang_exceptions::kErrorDetailsIndent);
            output << current_line_ << std::endl;
            output << *utils::MultiplyStr(" ", e.column()) << '^' << std::endl;
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
    ast::Program program = parser.ParseProgram();

    output << program.String();
    output << '\n';
}

void REPL::Eval(std::istream& input, std::ostream& output) {
    Lexer lexer{current_line_};
    Parser parser{lexer};
    ast::Program program = parser.ParseProgram();

    std::ostringstream out;
    evaluator_.Evaluate(program, input, out);
    
    std::string printed = out.str();
    if (printed.empty()) {
        output << evaluator_.GetLastEvaluatedValue();
    } else {
        output << printed;
    }

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
