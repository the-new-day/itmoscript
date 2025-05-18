#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <format>

#include "lib/interpreter.hpp"

using Value = ItmoScript::Value;

static std::string GetPrintableParserErrors(ItmoScript::Parser& parser) {
    const auto& errors = parser.GetErrors();

    std::ostringstream msg;

    if (!errors.empty()) {
        msg << "parser has " << errors.size() << " errors:\n";
        for (const auto& error : errors) {
            msg << std::format("Ln {}, Col {}: {}", error.token.line, error.token.column, error.message) << '\n';
        }
    }

    return msg.str();
}

static void CheckParserErrors(ItmoScript::Parser& parser, int expected_amount = 0) {
    std::string errors = GetPrintableParserErrors(parser);
    ASSERT_EQ(parser.GetErrors().size(), expected_amount) << errors;
}

static ItmoScript::Program GetParsedProgram(const std::string& code) {
    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();
    CheckParserErrors(parser);
    return program;
}

static ItmoScript::Value Eval(const std::string& input) {
    ItmoScript::Evaluator evaluator;
    ItmoScript::Program program = GetParsedProgram(input);
    evaluator.Interpret(program);
    return evaluator.GetResult();
}

static void TestInteger(const Value& value, int64_t expected) {
    ASSERT_TRUE(value.IsInt()) 
        << "real type is: " << ItmoScript::kValueTypeNames.at(value.GetType());

    ASSERT_EQ(value.GetValue<ItmoScript::Int>(), expected);
}

static void TestBool(const Value& value, bool expected) {
    ASSERT_TRUE(value.IsBool()) 
        << "real type is: " << ItmoScript::kValueTypeNames.at(value.GetType());

    ASSERT_EQ(value.GetValue<ItmoScript::Bool>(), expected);
}

static void TestNullType(const Value& value) {
    ASSERT_TRUE(value.IsNullType()) 
        << "real type is: " << ItmoScript::kValueTypeNames.at(value.GetType());
}
