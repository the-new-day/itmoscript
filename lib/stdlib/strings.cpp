#include "strings.hpp"
#include "StdLib.hpp"
#include "objects/List.hpp"
#include "utils.hpp"

#include <algorithm>

namespace itmoscript {

namespace stdlib {

namespace strings {

void RegisterAll(StdLib& lib) {
    lib.Register("lower", MakeBuiltin(Lower, 1));
    lib.Register("upper", MakeBuiltin(Upper, 1));
    lib.Register("split", MakeBuiltin(Split, 2));
    lib.Register("join", MakeBuiltin(Join, 2));
    lib.Register("replace", MakeBuiltin(Replace, 3));
}

Value Lower(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);
    const String& original = args[0].Get<String>();
    std::string lowered;
    lowered.reserve(original->size());

    std::transform(
        original->begin(), 
        original->end(), 
        std::back_inserter(lowered),
        [](char ch) { return std::tolower(ch); }
    );

    return CreateString(lowered);
}

Value Upper(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);
    const String& original = args[0].Get<String>();
    std::string uppered;
    uppered.reserve(original->size());

    std::transform(
        original->begin(), 
        original->end(), 
        std::back_inserter(uppered),
        [](char ch) { return std::toupper(ch); }
    );

    return CreateString(uppered);
}

Value Split(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);
    AssertType<String>(args[1], 1, from, call_stack);

    const std::string& str = *args[0].Get<String>();
    const std::string& delim = *args[1].Get<String>();

    if (delim.empty()) {
        return CreateList(std::vector<Value>{CreateString(str)});
    }

    std::vector<Value> result;
    size_t start = 0;
    size_t end = str.find(delim);

    while (end != std::string::npos) {
        result.push_back(CreateString(str.substr(start, end - start)));
        start = end + delim.length();
        end = str.find(delim, start);
    }

    result.push_back(CreateString(str.substr(start)));
    return CreateList(std::move(result));
}

Value Join(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<List>(args[0], 0, from, call_stack);
    AssertType<String>(args[1], 1, from, call_stack);

    std::string result = utils::Join<Value, std::string>(
        args[0].Get<List>()->data(),
        *args[1].Get<String>(),
        [](const Value& val) {
            return val.ToString();
        }
    );

    return CreateString(std::move(result));
}

Value Replace(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);
    AssertType<String>(args[1], 1, from, call_stack);
    AssertType<String>(args[2], 2, from, call_stack);

    const std::string& original = *args[0].Get<String>();
    const std::string& old = *args[1].Get<String>();
    const std::string& to = *args[2].Get<String>();

    return CreateString(utils::ReplaceAll(original, old, to));
}

} // namespace lists
    
} // namespace stdlib
    
} // namespace itmoscript
