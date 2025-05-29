#pragma once

#include "objects/Value.hpp"
#include "exceptions/ArgumentTypeError.hpp"

namespace itmoscript {

namespace stdlib {

class StdLib;

namespace numbers {

void RegisterAll(StdLib& lib);

Value Abs(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Ceil(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Floor(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Round(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Sqrt(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Rnd(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value ParseNum(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value ToString(const std::vector<Value>& args, Token from, const CallStack& call_stack);
    
} // namespace math

} // namespace stdlib
    
} // namespace itmoscript
