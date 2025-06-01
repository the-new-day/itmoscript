#pragma once

#include "objects/Value.hpp"
#include "exceptions/ArgumentTypeError.hpp"

namespace itmoscript {

namespace stdlib {

class StdLib;

namespace files {

void RegisterAll(StdLib& lib);

Value FileRead(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value FileReadLines(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value FileWrite(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value FileAppend(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value FileExists(const std::vector<Value>& args, Token from, const CallStack& call_stack);
    
} // namespace files

} // namespace stdlib
    
} // namespace itmoscript

