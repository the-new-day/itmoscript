#include "files.hpp"

#include "StdLib.hpp"
#include "objects/List.hpp"

#include "exceptions/FileAccessError.hpp"

#include <fstream>
#include <sstream>

namespace itmoscript {

namespace stdlib {

namespace files {
    
void RegisterAll(StdLib& lib) {
    lib.Register("file_read", MakeBuiltin("file_read", FileRead, 1));
    lib.Register("file_read_lines", MakeBuiltin("file_read_lines", FileReadLines, 1));
    lib.Register("file_write", MakeBuiltin("file_write", FileWrite, 2));
    lib.Register("file_append", MakeBuiltin("file_append", FileAppend, 2));
}

Value FileRead(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);

    const std::string& filename = *args[0].Get<String>();
    std::ifstream file{filename};

    if (!file.good()) {
        ThrowError<lang_exceptions::FileAccessError>(from, call_stack, filename);
    }

    std::ostringstream buf;
    buf << file.rdbuf();
    return CreateString(buf.str());
}

Value FileReadLines(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);

    const std::string& filename = *args[0].Get<String>();
    std::ifstream file{filename};

    if (!file.good()) {
        ThrowError<lang_exceptions::FileAccessError>(from, call_stack, filename);
    }

    std::string line;
    std::vector<Value> result;

    while (std::getline(file, line)) {
        result.push_back(CreateString(std::move(line)));
    }

    return CreateList(std::move(result));
}

Value FileWrite(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);

    const std::string& filename = *args[0].Get<String>();
    std::ofstream file{filename};

    if (!file.good()) {
        ThrowError<lang_exceptions::FileAccessError>(from, call_stack, filename);
    }

    if (args[1].IsOfType<String>()) {
        file << *args[1].Get<String>();
    } else {
        file << args[1].ToString();
    }

    return NullType{};
}

Value FileAppend(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<String>(args[0], 0, from, call_stack);

    const std::string& filename = *args[0].Get<String>();
    std::ofstream file{filename, std::ios::app};

    if (!file.good()) {
        ThrowError<lang_exceptions::FileAccessError>(from, call_stack, filename);
    }

    if (args[1].IsOfType<String>()) {
        file << *args[1].Get<String>();
    } else {
        file << args[1].ToString();
    }
    
    return NullType{};
}

} // namespace files
    
} // namespace stdlib
    
} // namespace itmoscript
