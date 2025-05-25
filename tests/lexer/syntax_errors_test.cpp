#include "lexer_test.hpp"

#include "lexer/SyntaxError.hpp"

TEST(LexerTestSuite, IllegalCharTest) {
    std::vector<std::string> codes = {
        R"(####)",
        R"(@@@@)",
        R"({{}{{{}{}}}})",
    };

    for (const auto& code : codes) {
        itmoscript::Lexer lexer{code};
        ASSERT_THROW(lexer.GetNextToken(), itmoscript::lang_exceptions::SyntaxError) 
            << "code: " << code;
    }
}

TEST(LexerTestSuite, UnexpectedEndOfStringTest) {
    std::vector<std::string> codes = {
        R"("asasasa)",
        R"(")",
        R"("####2423.@@@$$$$$!P@1)",
    };

    for (const auto& code : codes) {
        itmoscript::Lexer lexer{code};
        ASSERT_THROW(lexer.GetNextToken(), itmoscript::lang_exceptions::SyntaxError) 
            << "code: " << code;
    }
}

TEST(LexerTestSuite, UnexpectedEndOfNumberTest) {
    std::vector<std::string> codes = {
        "3333a",
        "333.33a",
    };

    for (const auto& code : codes) {
        itmoscript::Lexer lexer{code};
        ASSERT_THROW(lexer.GetNextToken(), itmoscript::lang_exceptions::SyntaxError) 
            << "code: " << code;
    }
}
