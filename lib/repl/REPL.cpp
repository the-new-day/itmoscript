#include "REPL.hpp"

#include <format>

namespace ItmoScript {

void REPL::Start(std::istream& input, std::ostream& output) {
    while (true) { 
        output << kReplPrompt;
        std::string line;

        if (!std::getline(input, line)) break;
        if (line.empty()) continue;

        if (mode_ == ReplMode::kLexer) {
            EvalLexer(line, output);
        } else if (mode_ == ReplMode::kParser) {
            EvalParser(line, output);
        }
    }
}

void REPL::EvalLexer(const std::string& line, std::ostream& output) {
    Lexer lexer{line};
    Token token = lexer.GetNextToken();

    while (token.type != TokenType::kEOF) {
        PrintToken(output, token);
        output << std::endl;
        token = lexer.GetNextToken();
    }
}

void REPL::EvalParser(const std::string& line, std::ostream& output) {
    Lexer lexer{line};
    Parser parser{lexer};
    Program program = parser.ParseProgram();

    if (parser.GetErrors().size() != 0) {
        PrintParserErrors(parser, output);
        return;
    }

    output << program.String();
    output << '\n';
}

void REPL::PrintToken(std::ostream& output, const Token& token) {
    output << std::format(
        "Ln {}, Col {} [{}(\"{}\")]",
        token.line,
        token.column,
        kTokenTypeNames.at(token.type),
        token.literal
    );
}

void REPL::PrintParserErrors(const Parser& parser, std::ostream& output) {
    output << "Parsing errors:\n";
    for (const auto& error : parser.GetErrors()) {
        output << "    ";
        PrintParserError(error, output);
    }
}

void REPL::PrintParserError(const ParserError& error, std::ostream& output) {
    output << std::format("Ln {}, Col {}: {}\n", error.token.line, error.token.column, error.message);
}

} // namespace ItmoScript
