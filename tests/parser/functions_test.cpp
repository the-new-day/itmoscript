#include "parser_test.hpp"

TEST(ParserTestSuite, SimpleFunctionTest) {
    std::string code = R"(
        function (x, y)
            x + y
        end function
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);
    ASSERT_NE(expr_stmt->expr, nullptr);

    auto* function_literal = dynamic_cast<ItmoScript::FunctionLiteral*>(expr_stmt->expr.get());
    ASSERT_NE(function_literal, nullptr);
    ASSERT_NE(function_literal->body, nullptr);

    ASSERT_EQ(function_literal->parameters.size(), 2);
    ASSERT_EQ(function_literal->parameters[0]->name, "x");
    ASSERT_EQ(function_literal->parameters[1]->name, "y");

    const auto& body_statements = function_literal->body->GetStatements();

    ASSERT_EQ(body_statements.size(), 1);

    auto* body = dynamic_cast<ItmoScript::ExpressionStatement*>(body_statements[0].get());
    ASSERT_NE(body->expr, nullptr);

    TestInfixExpression(body->expr, "x", "+", "y");
}

TEST(ParserTestSuite, FunctionParametersTest) {
    // <input, expected>
    std::vector<std::pair<std::string, std::vector<std::string>>> tests{
        {"function () end function", {}},
        {"function (x) end function", {"x"}},
        {"function (x, y, z) end function", {"x", "y", "z"}},
    };

    for (const auto& [input, expected] : tests) {
        ItmoScript::Lexer lexer{input};
        ItmoScript::Parser parser{lexer};
        ItmoScript::Program program = parser.ParseProgram();
        CheckParserErrors(parser);
        
        const auto& statements = program.GetStatements();
        ASSERT_EQ(statements.size(), 1);

        auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        auto* function_literal = dynamic_cast<ItmoScript::FunctionLiteral*>(expr_stmt->expr.get());
        ASSERT_NE(function_literal, nullptr);
        ASSERT_NE(function_literal->body, nullptr);

        ASSERT_EQ(function_literal->parameters.size(), expected.size());

        for (size_t i = 0; i < expected.size(); ++i) {
            ASSERT_EQ(function_literal->parameters[i]->name, expected[i]);
        }
    }
}

TEST(ParserTestSuite, FunctionCallTest) {
    std::string code = "add(1, 2 * 3, 4 + 5)";
    
    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);
    
    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);
    ASSERT_NE(expr_stmt->expr, nullptr);

    auto* call_expression = dynamic_cast<ItmoScript::CallExpression*>(expr_stmt->expr.get());
    ASSERT_NE(call_expression, nullptr);
    ASSERT_NE(call_expression->function, nullptr);

    TestIdentifier(call_expression->function, "add");

    ASSERT_EQ(call_expression->arguments.size(), 3);
    TestLiteralExpression(call_expression->arguments[0], 1);
    TestInfixExpression(call_expression->arguments[1], 2, "*", 3);
    TestInfixExpression(call_expression->arguments[2], 4, "+", 5);
}

struct FnCall {
    std::string input;
    std::string expected_ident;
    std::vector<std::string> expected_args;
};

TEST(ParserTestSuite, FunctionCallParametersTest) {
    std::vector<FnCall> tests{
        {"add()", "add", {}},
        {"add(1)", "add", {"1"}},
        {"add(1, 2 * 3, 4 + 5)", "add", {"1", "(2 * 3)", "(4 + 5)"}},
    };
    
    for (const auto& [input, ident, args] : tests) {
        ItmoScript::Lexer lexer{input};
        ItmoScript::Parser parser{lexer};
        ItmoScript::Program program = parser.ParseProgram();
        CheckParserErrors(parser);
        
        const auto& statements = program.GetStatements();
        ASSERT_EQ(statements.size(), 1);

        auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        auto* call_expression = dynamic_cast<ItmoScript::CallExpression*>(expr_stmt->expr.get());
        ASSERT_NE(call_expression, nullptr);
        ASSERT_NE(call_expression->function, nullptr);

        TestIdentifier(call_expression->function, ident);

        ASSERT_EQ(call_expression->arguments.size(), args.size());
        
        for (size_t i = 0; i < args.size(); ++i) {
            ASSERT_EQ(call_expression->arguments[i]->String(), args[i]);
        }
    }
}
