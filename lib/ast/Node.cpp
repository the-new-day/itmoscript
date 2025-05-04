#include "Node.hpp"

namespace ItmoScript {

std::string Program::GetTokenLiteral() const {
    if (!statements_.empty()) {
        return statements_[0]->token.literal;
    }

    return std::string{};
}

const std::vector<std::unique_ptr<Statement>>& Program::GetStatements() const {
    return statements_;
}

void Program::AddStatement(std::unique_ptr<Statement> statement) {
    statements_.push_back(std::move(statement));
}

} // namespace ItmoScript
