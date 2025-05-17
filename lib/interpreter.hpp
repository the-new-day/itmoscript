#include <iostream>

#include "lexer/Lexer.hpp"
#include "repl/REPL.hpp"
#include "parser/Parser.hpp"
#include "evaluation/Evaluator.hpp"

bool interpret(std::istream& input, std::ostream& output);