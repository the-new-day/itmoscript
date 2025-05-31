#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "lib/interpreter.hpp"

static itmoscript::ast::Program GetParsedProgram(const std::string& code) {
    itmoscript::Lexer lexer{code};
    itmoscript::Parser parser{lexer};
    itmoscript::ast::Program program = parser.ParseProgram();
    return program;
}

static itmoscript::Value Eval(const std::string& input) {
    itmoscript::Evaluator evaluator;
    itmoscript::ast::Program program = GetParsedProgram(input);
    evaluator.EnableStandardOperators();
    evaluator.EnableStd();
    std::stringstream dummy;
    evaluator.Evaluate(program, dummy, dummy);
    return evaluator.GetLastEvaluatedValue();
}
