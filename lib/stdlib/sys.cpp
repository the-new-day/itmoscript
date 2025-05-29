#include "sys.hpp"

#include "StdLib.hpp"
#include "objects/List.hpp"
#include "utils.hpp"
#include "Exception.hpp"

namespace itmoscript {

namespace stdlib {

namespace sys {

void RegisterAll(StdLib& lib) {
    lib.RegisterOutStreamHandlingFunc("print", MakeBuiltin(Print, 1));
    lib.RegisterOutStreamHandlingFunc("println", MakeBuiltin(PrintLn, 1));
    lib.RegisterInStreamHandlingFunc("read", MakeBuiltin(Read, 0));
    lib.Register("stacktrace", MakeBuiltin(Stacktrace, 0));
}

Value Print(
    std::ostream& os, 
    const std::vector<Value>& args, 
    Token from, 
    const CallStack& call_stack
) {
    os << args[0].ToString();
    return NullType{};
}

Value PrintLn(
    std::ostream& os, 
    const std::vector<Value>& args, 
    Token from, 
    const CallStack& call_stack
) {
    os << args[0].ToString() << std::endl;
    return NullType{};
}

Value Read(
    std::istream& is, 
    const std::vector<Value>& args, 
    Token from, 
    const CallStack& call_stack
) {
    std::string input;
    std::getline(is, input);
    return CreateString(std::move(input));
}

Value Stacktrace(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    std::vector<Value> result;
    result.reserve(call_stack.size());

    for (const CallFrame& frame : call_stack) {
        std::vector<Value> pair(2);
        pair[0] = CreateString(frame.function_name);
        pair[1] = static_cast<Int>(frame.entry_token.line);
        result.push_back(CreateList(std::move(pair)));
    }

    return CreateList(std::move(result));
}

} // namespace lists
    
} // namespace stdlib
    
} // namespace itmoscript
