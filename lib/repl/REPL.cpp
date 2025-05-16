#include "REPL.hpp"

#include <format>

namespace ItmoScript {

void REPL::Start(std::istream& input, std::ostream& output) {
    while (true) { 
        output << kReplPrompt;

        if (!std::getline(input, current_line_)) break;
        if (current_line_.empty()) continue;

        if (mode_ == ReplMode::kLexer) {
            EvalLexer(output);
        } else if (mode_ == ReplMode::kParser) {
            EvalParser(output);
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
        PrintParserError(error, output, 4);
        output << '\n';
    }
}

void REPL::PrintParserError(const ParserError& error, std::ostream& output, size_t indent) {
    std::string pos_info = std::format("Ln {}, Col {}: ", error.token.line, error.token.column);
    output << pos_info << error.message << '\n';

    for (size_t i = 0; i < pos_info.size() + indent; ++i) {
        output << ' ';
    }

    output << current_line_ << '\n';

    for (size_t i = 0; i < pos_info.size() + error.token.column + indent; ++i) {
        output << ' ';
    }

    output << '^';
}

} // namespace ItmoScript
