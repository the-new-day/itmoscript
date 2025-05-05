#include "parser_test.hpp"

TEST(ParserTestSuite, SimpleVariableAssignment) {
    std::string code = R"(
        x = 5
        y = 10
        foobar = 100500
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    const auto& statements = program.GetStatements();

    ASSERT_EQ(statements.size(), 3);

    std::vector<std::string> expected_identifiers = {
        "x",
        "y",
        "foobar"
    };

    for (size_t i = 0; i < statements.size(); ++i) {
        auto* assign_stmt = dynamic_cast<ItmoScript::AssignStatement*>(statements[i].get());
        ASSERT_NE(assign_stmt, nullptr) << "Statement " << i << " is not an AssignStatement";

        ASSERT_EQ(assign_stmt->ident->name, expected_identifiers[i]) 
            << "Identifier at position " << i << " doesn't match";

        // TODO: add check for value
    }

    CheckParserErrors(parser);
}

TEST(ParserTestSuite, SimpleVariableAssignmentErrors) {
    std::string code = R"(
        x #= 5
        y $= 10
        five = 6
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    CheckParserErrors(parser, 4);
}

TEST(ParserTestSuite, SimpleReturn) {
    std::string code = R"(
        return 5
        return 10
        return x
    )";

    ItmoScript::Lexer lexer{code};
    ItmoScript::Parser parser{lexer};
    ItmoScript::Program program = parser.ParseProgram();

    const auto& statements = program.GetStatements();

    ASSERT_EQ(statements.size(), 3);

    for (size_t i = 0; i < statements.size(); ++i) {
        auto* return_stmt = dynamic_cast<ItmoScript::ReturnStatement*>(statements[i].get());
        ASSERT_NE(return_stmt, nullptr) << "Statement " << i << " is not a ReturnStatement";

        // TODO: add check for value
    }
    
    CheckParserErrors(parser);
}
