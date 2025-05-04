#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>

#include "lib/interpreter.hpp"

using TT = ItmoScript::TokenType;

static void PrintParserErrors(ItmoScript::Parser& parser) {
    const auto& errors = parser.GetErrors();

    std::ostringstream msg;

    if (!errors.empty()) {
        msg << "parser has " << errors.size() << " errors:\n";
        for (const auto& error : errors) {
            msg << error << '\n';
        }
    }

    ASSERT_TRUE(errors.empty()) << msg.str();
}

static void TestIntegerLiteral(std::unique_ptr<ItmoScript::Expression>& int_literal_expr, int64_t expected_value) {
    auto* integer_literal = dynamic_cast<ItmoScript::IntegerLiteral*>(int_literal_expr.get());
    ASSERT_NE(integer_literal, nullptr);

    int64_t int_value = integer_literal->value;
    ASSERT_EQ(int_value, expected_value);
    ASSERT_EQ(integer_literal->token.literal, std::to_string(expected_value)); // should work for bin, hex, octal literals too
}

static void TestIdentifier(std::unique_ptr<ItmoScript::Expression>& ident_expr, const std::string& expected_value) {
    auto* ident = dynamic_cast<ItmoScript::Identifier*>(ident_expr.get());
    ASSERT_NE(ident, nullptr);

    std::string name = ident->name;
    ASSERT_EQ(name, expected_value);
    ASSERT_EQ(ident->token.literal, expected_value);
}

template<typename T>
void TestLiteralExpression(std::unique_ptr<ItmoScript::Expression>& expr, T expected) {
    if constexpr (std::is_same_v<T, int64_t>) {
        TestIntegerLiteral(expr, expected);
    } else if constexpr (std::is_same_v<T, std::string>) {
        TestIdentifier(expr, expected);
    } else {
        static_assert(false, "Unsupported type for TestLiteralExpression");
    }
}

template<typename L, typename R>
void TestInfixExpression(std::unique_ptr<ItmoScript::Expression>& expr, L left, std::string oper, R right) {
    auto* infix_expr = dynamic_cast<ItmoScript::InfixExpression*>(expr.get());
    ASSERT_NE(infix_expr, nullptr);
    ASSERT_NE(infix_expr->right, nullptr);
    ASSERT_NE(infix_expr->left, nullptr);
    ASSERT_EQ(infix_expr->oper, oper);

    TestLiteralExpression(infix_expr->left, left);
    TestLiteralExpression(infix_expr->right, right);
}
