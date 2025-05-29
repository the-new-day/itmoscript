#pragma once

#include "objects/Value.hpp"
#include "exceptions/ArgumentTypeError.hpp"

#include <iostream>

namespace itmoscript {

namespace stdlib {

class StdLib;

namespace sys {

void RegisterAll(StdLib& lib);

Value Print(std::ostream& os, const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value PrintLn(std::ostream& os, const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Read(std::istream& is, const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Stacktrace(const std::vector<Value>& args, Token from, const CallStack& call_stack);
    
} // namespace math

} // namespace stdlib
    
} // namespace itmoscript
