#include "objects/Value.hpp"
#include "exceptions/ArgumentTypeError.hpp"

namespace itmoscript {

namespace stdlib {

class StdLib;

namespace strings {

void RegisterAll(StdLib& lib);

Value Lower(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Upper(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Split(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Join(const std::vector<Value>& args, Token from, const CallStack& call_stack);
Value Replace(std::vector<Value>& args, Token from, const CallStack& call_stack);

} // namespace lists
    
} // namespace stdlib
    
} // namespace itmoscript
