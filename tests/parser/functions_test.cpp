#include "parser_test.hpp"

// Code must contain only function literal
itmoscript::ast::FunctionLiteral* GetFuncLiteral(const itmoscript::ast::Program& program) {
    const auto& statements = program.GetStatements();
    EXPECT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    auto* function_literal = dynamic_cast<itmoscript::ast::FunctionLiteral*>(expr_stmt->expr.get());
    EXPECT_NE(function_literal, nullptr);
    EXPECT_NE(function_literal->body, nullptr);

    return function_literal;
}

// Code must contain only call expression
itmoscript::ast::CallExpression* GetCallExpr(const itmoscript::ast::Program& program) {
    const auto& statements = program.GetStatements();
    EXPECT_EQ(statements.size(), 1);

    auto* expr_stmt = GetExpressionStatement(statements[0]);
    auto* call_expr = dynamic_cast<itmoscript::ast::CallExpression*>(expr_stmt->expr.get());
    EXPECT_NE(call_expr, nullptr);
    EXPECT_NE(call_expr->function, nullptr);

    return call_expr;
}

TEST(ParserFunctionsTestSuite, FunctionSimpleTest) {
    std::string code = R"(
        function (x, y)
            x + y
        end function
    )";

    auto program = GetParsedProgram(code);
    auto* function_literal = GetFuncLiteral(program);

    ASSERT_EQ(function_literal->parameters.size(), 2);
    ASSERT_EQ(function_literal->parameters[0]->name, "x");
    ASSERT_EQ(function_literal->parameters[1]->name, "y");

    const auto& body_statements = function_literal->body->GetStatements();
    ASSERT_EQ(body_statements.size(), 1);

    auto* body = GetExpressionStatement(body_statements[0]);
    TestInfixExpression(body->expr, "x", itmoscript::TokenType::kPlus, "y");
}

TEST(ParserFunctionsTestSuite, FunctionParametersTest) {
    // <input, expected>
    std::vector<std::pair<std::string, std::vector<std::string>>> tests{
        {"function () end function", {}},
        {"function (x) end function", {"x"}},
        {"function (x, y, z) end function", {"x", "y", "z"}},
    };

    for (const auto& [input, expected] : tests) {
        auto program = GetParsedProgram(input);
        auto* function_literal = GetFuncLiteral(program);

        ASSERT_EQ(function_literal->parameters.size(), expected.size());

        for (size_t i = 0; i < expected.size(); ++i) {
            ASSERT_EQ(function_literal->parameters[i]->name, expected[i]);
        }
    }
}

TEST(ParserFunctionsTestSuite, FunctionCallTest) {
    std::string code = "add(1, 2 * 3, 4 + 5)";
    
    auto program = GetParsedProgram(code);
    auto* call_expression = GetCallExpr(program);

    TestIdentifier(call_expression->function, "add");

    ASSERT_EQ(call_expression->arguments.size(), 3);
    TestLiteralExpression(call_expression->arguments[0], 1);
    TestInfixExpression(call_expression->arguments[1], 2, itmoscript::TokenType::kAsterisk, 3);
    TestInfixExpression(call_expression->arguments[2], 4, itmoscript::TokenType::kPlus, 5);
}

struct FnCall {
    std::string input;
    std::string expected_ident;
    std::vector<std::string> expected_args;
};

TEST(ParserFunctionsTestSuite, FunctionCallParametersTest) {
    std::vector<FnCall> tests{
        {"add()", "add", {}},
        {"add(1)", "add", {"1"}},
        {"add(1, 2 * 3, 4 + 5)", "add", {"1", "(2 * 3)", "(4 + 5)"}},
    };
    
    for (const auto& [input, ident, args] : tests) {
        auto program = GetParsedProgram(input);
        auto* call_expression = GetCallExpr(program);

        TestIdentifier(call_expression->function, ident);
        ASSERT_EQ(call_expression->arguments.size(), args.size());
        
        for (size_t i = 0; i < args.size(); ++i) {
            ASSERT_EQ(call_expression->arguments[i]->String(), args[i]);
        }
    }
}
