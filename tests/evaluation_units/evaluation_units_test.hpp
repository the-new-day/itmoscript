#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <format>

#include "lib/interpreter.hpp"

using IsValue = itmoscript::Value;
using IsValueType = itmoscript::ValueType;

static itmoscript::ast::Program GetParsedProgram(const std::string& code) {
    itmoscript::Lexer lexer{code};
    itmoscript::Parser parser{lexer};
    itmoscript::ast::Program program = parser.ParseProgram();
    return program;
}

static itmoscript::Value Eval(const std::string& input) {
    itmoscript::Evaluator evaluator;
    itmoscript::ast::Program program = GetParsedProgram(input);
    evaluator.Interpret(program);
    return evaluator.GetLastEvaluatedValue();
}

template<typename T>
void TestValue(const IsValue& value, T expected) {
    ASSERT_TRUE(value.IsOfType<T>()) 
        << "real type of " << value.ToString() << 
        " is: " << itmoscript::GetTypeName(value.type());
    
    ASSERT_EQ(value.Get<T>(), expected);
}

template<typename T, typename Inner>
void TestHeavyValue(const IsValue& value, const std::shared_ptr<Inner>& expected) {
    ASSERT_TRUE(value.IsOfType<T>()) 
        << "real type of " << value.ToString() << 
        " is: " << itmoscript::GetTypeName(value.type());
    
    ASSERT_EQ(*value.Get<T>(), *expected);
}

template<typename T, typename InnerType>
std::shared_ptr<InnerType> CreateHeavyValue(InnerType val) {
    return std::make_shared<InnerType>(std::move(val));
}

static std::shared_ptr<std::string> CreateString(std::string val) {
    return CreateHeavyValue<itmoscript::String>(std::move(val));
}

static std::shared_ptr<itmoscript::ListObject> CreateList(itmoscript::ListObject val) {
    return CreateHeavyValue<itmoscript::List>(std::move(val));
}
