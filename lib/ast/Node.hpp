#pragma once

#include <string>
#include <memory>
#include <vector>

#include "lexer/Lexer.hpp"

namespace ItmoScript {

struct Node {
    Node(const Token& token)
        : token(token) {}
    
    Node() = default;

    Token token;
    virtual ~Node() = default;
};

struct Statement : public Node {
    using Node::Node;
};

struct Expression : public Node {
    using Node::Node;
};

struct Identifier : public Expression {
    using Expression::Expression;
    std::string value;
};

struct AssignStatement : public Statement {
    using Statement::Statement;
    
    std::unique_ptr<Identifier> ident;
    std::unique_ptr<Expression> expr;
};

struct ReturnStatement : public Statement {
    using Statement::Statement;
    std::unique_ptr<Expression> expr;
};

class Program {
public:
    std::string GetTokenLiteral() const;
    const std::vector<std::unique_ptr<Statement>>& GetStatements() const;
    void AddStatement(std::unique_ptr<Statement> statement);

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};
    
} // namespace ItmoScript
