#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

#include "lexer/Lexer.hpp"
#include "AstVisitor.hpp"

namespace itmoscript {

namespace ast {

struct Node {
    Node(const Token& token)
        : token(token) {}
    Node() = default;

    virtual ~Node() = default;
    virtual std::string String() const { return token.literal; }
    virtual void Accept(AstVisitor& visitor) = 0;

    Token token;
};

struct Statement : public Node {
    using Node::Node;
};

struct Expression : public Node {
    using Node::Node;
};

class Program : public Node {
public:
    using Node::Node;
    const std::vector<std::shared_ptr<Statement>>& GetStatements() const;
    void AddStatement(std::shared_ptr<Statement> statement);

    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

private:
    std::vector<std::shared_ptr<Statement>> statements_;
};

struct ExpressionStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::shared_ptr<Expression> expr;
};

struct Identifier : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::string name;
};

struct AssignStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::shared_ptr<Identifier> ident;
    std::shared_ptr<Expression> expr;
};

struct OperatorAssignStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    TokenType oper;
    std::shared_ptr<Identifier> ident;
    std::shared_ptr<Expression> expr;
};

struct ReturnStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::shared_ptr<Expression> expr;
};

struct PrefixExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    TokenType oper;
    std::shared_ptr<Expression> right;
};

struct InfixExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    TokenType oper;
    std::shared_ptr<Expression> right;
    std::shared_ptr<Expression> left;
};

struct IntegerLiteral : public Expression {
    using Expression::Expression;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    int64_t value;
};

struct FloatLiteral : public Expression {
    using Expression::Expression;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    double value;
};

struct StringLiteral : public Expression {
    using Expression::Expression;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::string value;
};

struct BooleanLiteral : public Expression {
    using Expression::Expression;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    bool value;
};

struct NullTypeLiteral : public Expression {
    using Expression::Expression;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }
};

class BlockStatement : public Statement {
public:
    using Statement::Statement;
    const std::vector<std::shared_ptr<Statement>>& GetStatements() const;
    void AddStatement(std::shared_ptr<Statement> statement);

    std::string String() const;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

private:
    std::vector<std::shared_ptr<Statement>> statements_;
};

struct IfBranch {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockStatement> consequence;
    Token token;

    std::string String() const;
    IfBranch(const Token& token) : token(token) {}
    IfBranch() = default;
};

struct IfExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::vector<IfBranch> alternatives;
};

struct FunctionLiteral : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::vector<std::shared_ptr<Identifier>> parameters;
    std::shared_ptr<BlockStatement> body;
};

struct CallExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::shared_ptr<Expression> function; // Identifier or FunctionLiteral
    std::vector<std::shared_ptr<Expression>> arguments;
    std::optional<std::string> function_name; // Filled if function is an Identifier
};

struct WhileStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockStatement> body;
};

struct ForStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }

    std::shared_ptr<Identifier> iter;
    std::shared_ptr<Expression> range;
    std::shared_ptr<BlockStatement> body;
};

struct BreakStatement : public Statement {
    using Statement::Statement;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }
};

struct ContinueStatement : public Statement {
    using Statement::Statement;
    void Accept(AstVisitor& visitor) override { visitor.Visit(*this); }
};

} // namespace ast

} // namespace itmoscript
