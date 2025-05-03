#include "REPL.hpp"

#include <format>

namespace ItmoScript {

void REPL::Start(std::istream& input, std::ostream& output) {
    while (true) { 
        output << kReplPrompt;
        std::string line;

        if (!std::getline(input, line)) break;
        if (line.empty()) continue;

        Lexer lexer{line};
        Token token = lexer.GetNextToken();

        while (token.type != TokenType::kEOF) {
            PrintToken(output, token);
            output << std::endl;
            token = lexer.GetNextToken();
        }
    }
}

void REPL::PrintToken(std::ostream& output, const Token& token) {
    output << std::format("[Type: {}, Literal: \"{}\"]", kTokenTypeNames.at(token.type), token.literal);
}

} // namespace ItmoScript

