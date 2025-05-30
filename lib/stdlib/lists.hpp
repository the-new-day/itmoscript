#include "objects/Value.hpp"
#include "exceptions/ArgumentTypeError.hpp"

namespace itmoscript {

namespace stdlib {

class StdLib;

namespace lists {

void RegisterAll(StdLib& lib);

Value Len(const std::vector<Value>& args, Token from, const CallStack& call_stack);

Value Range(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Push(std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Pop(std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Insert(std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Remove(std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Sort(std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Set(std::vector<Value>& args, Token from, const CallStack& call_stack);

} // namespace lists
    
} // namespace stdlib
    
} // namespace itmoscript
