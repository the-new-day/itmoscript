#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

#include "lexer/Lexer.hpp"

namespace ItmoScript {

struct Node {
    Node(const Token& token)
        : token(token) {}
    Node() = default;

    virtual ~Node() = default;
    virtual std::string String() const = 0;

    Token token;
};

struct Statement : public Node {
    using Node::Node;
    std::string String() const override;
};

struct Expression : public Node {
    using Node::Node;
    std::string String() const override;
};

struct ExpressionStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;

    std::unique_ptr<Expression> expr;
};

struct Identifier : public Expression {
    using Expression::Expression;
    std::string String() const override;
    
    std::string name;
};

struct AssignStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    
    std::unique_ptr<Identifier> ident;
    std::unique_ptr<Expression> expr;
};

struct ReturnStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;

    std::unique_ptr<Expression> expr;
};

struct PrefixExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;

    std::string oper;
    std::unique_ptr<Expression> right;
};

struct InfixExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;

    std::string oper;
    std::unique_ptr<Expression> right;
    std::unique_ptr<Expression> left;
};

class Program {
public:
    std::string GetTokenLiteral() const;
    const std::vector<std::unique_ptr<Statement>>& GetStatements() const;
    void AddStatement(std::unique_ptr<Statement> statement);

    std::string String() const;

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

struct IntegerLiteral : public Expression {
    using Expression::Expression;
    std::string String() const override;

    int64_t value;
};

struct BooleanLiteral : public Expression {
    using Expression::Expression;
    std::string String() const override;
    
    bool value;
};
    
} // namespace ItmoScript
