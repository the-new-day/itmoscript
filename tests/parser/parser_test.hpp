#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>

#include "lib/interpreter.hpp"

using TT = itmoscript::TokenType;

template<typename T>
struct PrefixOpExpr {
    std::string input;
    std::string oper;
    T value;
};

template<typename L, typename R = L>
struct InfixOpExpr {
    std::string input;
    std::string oper;
    L left_value;
    R right_value;
};


static itmoscript::Program GetParsedProgram(const std::string& code) {
    itmoscript::Lexer lexer{code};
    itmoscript::Parser parser{lexer};
    itmoscript::Program program = parser.ParseProgram();
    return program;
}

static void TestIntegerLiteral(std::unique_ptr<itmoscript::Expression>& int_literal_expr, int64_t expected_value) {
    auto* literal = dynamic_cast<itmoscript::IntegerLiteral*>(int_literal_expr.get());
    ASSERT_NE(literal, nullptr);

    ASSERT_EQ(literal->value, expected_value);
    ASSERT_EQ(literal->token.literal, std::to_string(expected_value));
}

static void TestFloatLiteral(std::unique_ptr<itmoscript::Expression>& float_literal_expr, double expected_value) {
    auto* literal = dynamic_cast<itmoscript::FloatLiteral*>(float_literal_expr.get());
    ASSERT_NE(literal, nullptr);
    ASSERT_EQ(literal->value, expected_value);
}

static void TestStringLiteral(std::unique_ptr<itmoscript::Expression>& string_literal_expr, std::string expected_value) {
    auto* literal = dynamic_cast<itmoscript::StringLiteral*>(string_literal_expr.get());
    ASSERT_NE(literal, nullptr);
    ASSERT_EQ(literal->value, expected_value);
}

static void TestNullTypeLiteral(std::unique_ptr<itmoscript::Expression>& null_literal_expr) {
    auto* literal = dynamic_cast<itmoscript::NullTypeLiteral*>(null_literal_expr.get());
    ASSERT_NE(literal, nullptr);
}

static void TestIdentifier(std::unique_ptr<itmoscript::Expression>& ident_expr, const std::string& expected_value) {
    auto* ident = dynamic_cast<itmoscript::Identifier*>(ident_expr.get());
    ASSERT_NE(ident, nullptr);

    std::string name = ident->name;
    ASSERT_EQ(name, expected_value);
    ASSERT_EQ(ident->token.literal, expected_value);
}

static void TestBooleanLiteral(std::unique_ptr<itmoscript::Expression>& int_literal_expr, bool expected_value) {
    auto* bool_literal = dynamic_cast<itmoscript::BooleanLiteral*>(int_literal_expr.get());
    ASSERT_NE(bool_literal, nullptr);

    bool bool_value = bool_literal->value;
    ASSERT_EQ(bool_value, expected_value);
    ASSERT_EQ(bool_literal->token.literal, expected_value ? "true" : "false");
}

template<typename T>
void TestLiteralExpression(std::unique_ptr<itmoscript::Expression>& expr, T expected) {
    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>) {
        TestIdentifier(expr, expected);
    } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
        TestIntegerLiteral(expr, expected);
    } else if constexpr (std::is_same_v<T, bool>) {
        TestBooleanLiteral(expr, expected);
    } else {
        static_assert(false, "Unsupported type for TestLiteralExpression");
    }
}

template<typename L, typename R>
void TestInfixExpression(std::unique_ptr<itmoscript::Expression>& expr, L left, std::string oper, R right) {
    auto* infix_expr = dynamic_cast<itmoscript::InfixExpression*>(expr.get());
    ASSERT_NE(infix_expr, nullptr);
    ASSERT_NE(infix_expr->right, nullptr);
    ASSERT_NE(infix_expr->left, nullptr);
    ASSERT_EQ(infix_expr->oper, oper);

    TestLiteralExpression(infix_expr->left, left);
    TestLiteralExpression(infix_expr->right, right);
}

template<typename L, typename R = L>
void TestInfixLiteralsExpressions(const std::vector<InfixOpExpr<L, R>>& expressions) {
    for (const auto& test_expr : expressions) {
        auto program = GetParsedProgram(test_expr.input);

        const auto& statements = program.GetStatements();
        ASSERT_EQ(statements.size(), 1) << "parser returned more than 1 statement, got " << statements.size() << '\n';

        auto* expr_stmt = dynamic_cast<itmoscript::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        TestInfixExpression(expr_stmt->expr, test_expr.left_value, test_expr.oper, test_expr.right_value);
    }
}

template<typename L>
void TestPrefixLiteralsExpressions(const std::vector<PrefixOpExpr<L>>& expressions) {
    for (const auto& test_expr : expressions) {
        auto program = GetParsedProgram(test_expr.input);

        const auto& statements = program.GetStatements();
        ASSERT_EQ(statements.size(), 1);

        auto* expr_stmt = dynamic_cast<itmoscript::ExpressionStatement*>(statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        ASSERT_NE(expr_stmt->expr, nullptr);

        auto* prefix_expr = dynamic_cast<itmoscript::PrefixExpression*>(expr_stmt->expr.get());
        ASSERT_NE(prefix_expr, nullptr);
        ASSERT_NE(prefix_expr->right, nullptr);

        ASSERT_EQ(prefix_expr->oper, test_expr.oper);

        TestLiteralExpression(prefix_expr->right, test_expr.value);
    }
}

template<typename T>
void TestStatement(const std::unique_ptr<itmoscript::Statement>& stmt, const std::string& expected) {
    auto* body = dynamic_cast<T*>(stmt.get());
    ASSERT_NE(body, nullptr);
    ASSERT_EQ(body->String(), expected);
}

static itmoscript::ExpressionStatement* GetExpressionStatement(const std::unique_ptr<itmoscript::Statement>& stmt) {
    auto* expr_stmt = dynamic_cast<itmoscript::ExpressionStatement*>(stmt.get());
    EXPECT_NE(expr_stmt, nullptr);
    EXPECT_NE(expr_stmt->expr, nullptr);
    return expr_stmt;
}

static itmoscript::BlockStatement* GetBlockStatement(const std::unique_ptr<itmoscript::Statement>& stmt) {
    auto* block_stmt = dynamic_cast<itmoscript::BlockStatement*>(stmt.get());
    EXPECT_NE(block_stmt, nullptr);
    return block_stmt;
}
