#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <functional>

#include "ast/Visitor.hpp"
#include "ast/AST.hpp"
#include "evaluation/Value.hpp"

namespace ItmoScript {
    
class Evaluator : public Visitor {
public:
    void Interpret(Program& root);
    Value GetResult() const;

    void Visit(Program&) override;
    void Visit(IntegerLiteral&) override;
    void Visit(BooleanLiteral&) override;
    void Visit(ExpressionStatement&) override;

    void Visit(Identifier&) override {}
    void Visit(AssignStatement&) override {}
    void Visit(ReturnStatement&) override {}
    void Visit(PrefixExpression&) override {}
    void Visit(InfixExpression&) override {}
    void Visit(FloatLiteral&) override {}
    void Visit(StringLiteral&) override {}
    void Visit(BlockStatement&) override {}
    void Visit(IfExpression&) override {}
    void Visit(FunctionLiteral&) override {}
    void Visit(CallExpression&) override {}
    void Visit(WhileStatement&) override {}
    void Visit(ForStatement&) override {}
    void Visit(BreakStatement&) override {}
    void Visit(ContinueStatement&) override {}

private:
    Value result_;
    Value Eval(Node& node);
};
    
} // namespace ItmoScript
