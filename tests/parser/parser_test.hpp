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
