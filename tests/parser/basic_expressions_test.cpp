#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "lib/interpreter.hpp"

using TT = ItmoScript::TokenType;

TEST(ParserTestSuite, SimpleExpressionStringTest) {
    std::string code = R"(
        x1 = x2
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    ASSERT_EQ(program.String(), "x1 = x2\n");
}

TEST(ParserTestSuite, IdentifierExpressionTest) {
    std::string code = R"(
        foobar
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    std::string ident = expr_stmt->String();
    ASSERT_EQ(ident, "foobar");
}

TEST(ParserTestSuite, IntLiteralExpressionTest) {
    std::string code = R"(
        100500
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    const auto& statements = program.GetStatements();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ItmoScript::ExpressionStatement*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);
    ASSERT_NE(expr_stmt->expr, nullptr);

    std::string str_value = expr_stmt->String();
    ASSERT_EQ(str_value, "100500");

    auto* integer_literal = dynamic_cast<ItmoScript::IntegerLiteral*>(expr_stmt->expr.get());
    ASSERT_NE(integer_literal, nullptr);

    int64_t int_value = integer_literal->value;
    ASSERT_EQ(int_value, 100500);
}
