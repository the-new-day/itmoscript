#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

#include "lexer/Lexer.hpp"
#include "Visitor.hpp"

namespace itmoscript {

struct Node {
    Node(const Token& token)
        : token(token) {}
    Node() = default;

    virtual ~Node() = default;
    virtual std::string String() const { return token.literal; }
    virtual void Accept(Visitor& visitor) = 0;

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
    const std::vector<std::unique_ptr<Statement>>& GetStatements() const;
    void AddStatement(std::unique_ptr<Statement> statement);

    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

struct ExpressionStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::unique_ptr<Expression> expr;
};

struct Identifier : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
    
    std::string name;
};

struct AssignStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
    
    std::unique_ptr<Identifier> ident;
    std::unique_ptr<Expression> expr;
};

struct ReturnStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::unique_ptr<Expression> expr;
};

struct PrefixExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::string oper;
    std::unique_ptr<Expression> right;
};

struct InfixExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::string oper;
    std::unique_ptr<Expression> right;
    std::unique_ptr<Expression> left;
};

struct IntegerLiteral : public Expression {
    using Expression::Expression;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    int64_t value;
};

struct FloatLiteral : public Expression {
    using Expression::Expression;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    double value;
};

struct StringLiteral : public Expression {
    using Expression::Expression;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::string value;
};

struct BooleanLiteral : public Expression {
    using Expression::Expression;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
    
    bool value;
};

struct NullTypeLiteral : public Expression {
    using Expression::Expression;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
};

class BlockStatement : public Statement {
public:
    using Statement::Statement;
    const std::vector<std::unique_ptr<Statement>>& GetStatements() const;
    void AddStatement(std::unique_ptr<Statement> statement);

    std::string String() const;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
    
private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

struct IfBranch {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> consequence;
    Token token;

    std::string String() const;
    IfBranch(const Token& token) : token(token) {}
    IfBranch() = default;
};

struct IfExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::vector<IfBranch> alternatives;
};

struct FunctionLiteral : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<BlockStatement> body;
};

struct CallExpression : public Expression {
    using Expression::Expression;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::unique_ptr<Expression> function; // Identifier or FunctionLiteral
    std::vector<std::unique_ptr<Expression>> arguments;
};

struct WhileStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;
};

struct ForStatement : public Statement {
    using Statement::Statement;
    std::string String() const override;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }

    std::unique_ptr<Identifier> iter;
    std::unique_ptr<Expression> range;
    std::unique_ptr<BlockStatement> body;
};

struct BreakStatement : public Statement {
    using Statement::Statement;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
};

struct ContinueStatement : public Statement {
    using Statement::Statement;
    void Accept(Visitor& visitor) override { visitor.Visit(*this); }
};

} // namespace itmoscript
