#pragma once

namespace itmoscript {

namespace ast {

struct Node;
struct Program;
struct ExpressionStatement;
struct Identifier;
struct AssignStatement;
struct OperatorAssignStatement;
struct ReturnStatement;
struct PrefixExpression;
struct InfixExpression;
struct IntegerLiteral;
struct FloatLiteral;
struct StringLiteral;
struct BooleanLiteral;
struct NullTypeLiteral;
struct BlockStatement;
struct IfExpression;
struct FunctionLiteral;
struct CallExpression;
struct WhileStatement;
struct ForStatement;
struct BreakStatement;
struct ContinueStatement;
struct IndexOperatorExpression;
struct ListLiteral;

/**
 * @class AstVisitor
 * @brief Interface for all AST visitors. Allows polymorphic Accept() methods
 * in the nodes, implementing the Visitor pattern.
 */
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    
    virtual void Visit(Program&) = 0;

    virtual void Visit(AssignStatement&) = 0;
    virtual void Visit(OperatorAssignStatement&) = 0;
    virtual void Visit(ExpressionStatement&) = 0;
    
    virtual void Visit(Identifier&) = 0;
    virtual void Visit(PrefixExpression&) = 0;
    virtual void Visit(InfixExpression&) = 0;
    virtual void Visit(IndexOperatorExpression&) = 0;

    virtual void Visit(IntegerLiteral&) = 0;
    virtual void Visit(FloatLiteral&) = 0;
    virtual void Visit(StringLiteral&) = 0;
    virtual void Visit(BooleanLiteral&) = 0;
    virtual void Visit(NullTypeLiteral&) = 0;
    virtual void Visit(FunctionLiteral&) = 0;
    virtual void Visit(ListLiteral&) = 0;
    
    virtual void Visit(CallExpression&) = 0;
    virtual void Visit(ReturnStatement&) = 0;

    virtual void Visit(BlockStatement&) = 0;
    virtual void Visit(IfExpression&) = 0;
    virtual void Visit(WhileStatement&) = 0;
    virtual void Visit(ForStatement&) = 0;
    virtual void Visit(BreakStatement&) = 0;
    virtual void Visit(ContinueStatement&) = 0;

    friend struct Node;
};
    
} // namespace ast

} // namespace itmoscript
