#include "objects/Value.hpp"
#include "exceptions/ArgumentTypeError.hpp"

namespace itmoscript {

namespace stdlib {

class StdLib;

namespace lists {

void RegisterAll(StdLib& lib);

Value Range(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Len(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Push(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Pop(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Insert(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Remove(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Sort(const std::vector<Value>& args, Token from, const CallStack& call_stack);

} // namespace lists
    
} // namespace stdlib
    
} // namespace itmoscript
