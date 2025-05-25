#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <format>

#include "lib/interpreter.hpp"

using IsValue = itmoscript::Value;
using IsValueType = itmoscript::ValueType;

static itmoscript::Program GetParsedProgram(const std::string& code) {
    itmoscript::Lexer lexer{code};
    itmoscript::Parser parser{lexer};
    itmoscript::Program program = parser.ParseProgram();
    return program;
}

static itmoscript::Value Eval(const std::string& input) {
    itmoscript::Evaluator evaluator;
    itmoscript::Program program = GetParsedProgram(input);
    evaluator.Interpret(program);
    return evaluator.GetLastEvaluatedValue();
}

template<typename T>
void TestValue(const IsValue& value, T expected) {
    ASSERT_TRUE(value.IsOfType<T>()) 
        << "real type of " << value.ToString() << " is: " << itmoscript::kValueTypeNames.at(value.type());
    
    ASSERT_EQ(value.Get<T>(), expected);
}
