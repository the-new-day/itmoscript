#include "lists.hpp"
#include "StdLib.hpp"
#include "objects/List.hpp"

#include "exceptions/EmptyListPopError.hpp"
#include "exceptions/InvalidArgumentError.hpp"
#include "evaluation/exceptions/IndexOutOfRangeError.hpp"

#include <algorithm>

namespace itmoscript {

namespace stdlib {

namespace lists {
    
void RegisterAll(StdLib& lib) {
    lib.Register("len", MakeBuiltin("len", Len, 1));
    lib.Register("push", MakeBuiltin("push", Push, 2));
    lib.Register("pop", MakeBuiltin("pop", Pop, 1));
    lib.Register("insert", MakeBuiltin("insert", Insert, 3));
    lib.Register("remove", MakeBuiltin("remove", Remove, 2));
    lib.Register("range", Range);
    lib.Register("sort", MakeBuiltin("sort", Sort, 1));
    lib.Register("set", MakeBuiltin("set", Set, 3));
}

Value Len(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    const Value& arg = args[0];

    if (!arg.IsOfType<List>() && !arg.IsOfType<String>()) {
        ThrowArgumentTypeError(std::move(from), call_stack, 0, arg.GetType(), "List or String");
    }

    if (arg.IsOfType<List>()) {
        return static_cast<Int>(arg.Get<List>()->size());
    } else {
        return static_cast<Int>(arg.Get<String>()->size());
    }
}

Value Range(const std::vector<Value>& args, Token from, const CallStack& call_stack) {
    if (args.size() == 0 || args.size() > 3) {
        throw lang_exceptions::ParametersCountError{from, call_stack, "range", 1, args.size()};
    }

    for (size_t i = 0; i < args.size(); ++i) {
        AssertType<Int>(args[i], i, from, call_stack);
    }

    Int start = 0;
    Int end;
    Int step = 1;

    if (args.size() == 1) {
        end = args[0].Get<Int>();
    } else if (args.size() == 2) {
        start = args[0].Get<Int>();
        end = args[1].Get<Int>();
    } else if (args.size() == 3) {
        start = args[0].Get<Int>();
        end = args[1].Get<Int>();
        step = args[2].Get<Int>();
    }

    if (step == 0) {
        ThrowError<lang_exceptions::InvalidArgumentError>(
            std::move(from),
            call_stack,
            2uz,
            "step in range() can't be zero"
        );
    }

    std::vector<Value> result;
    result.reserve(std::abs(end - start) / std::abs(step));

    if (step > 0) {
        for (int64_t i = 0; start + i * step < end; ++i) {
            result.push_back(Value{start + i * step});
        }
    } else {
        for (int64_t i = 0; start + i * step > end; ++i) {
            result.push_back(Value{start + i * step});
        }
    }

    return CreateList(result);
}

Value Push(std::vector<Value>& args, Token from, const CallStack& call_stack) {
    List& list = args[0].Get<List>();
    list->Insert(list->size(), std::move(args[1]));
    return list;
}

Value Pop(std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<List>(args[0], 0, from, call_stack);
    List& list = args[0].Get<List>();

    if (list->size() == 0) {
        ThrowError<lang_exceptions::EmptyListPopError>(std::move(from), call_stack);
    }

    list->Remove(list->size() - 1);
    return list;
}

Value Insert(std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<List>(args[0], 0, from, call_stack);
    AssertType<Int>(args[1], 1, from, call_stack);

    List& list = args[0].Get<List>();
    Int index = args[1].Get<Int>();

    if (index < 0 || index > list->size()) {
        ThrowError<lang_exceptions::IndexOutOfRangeError>(
            std::move(from), call_stack, index, list->size()
        );
    }

    list->Insert(index, std::move(args[2]));
    return list;
}

Value Remove(std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<List>(args[0], 0, from, call_stack);
    AssertType<Int>(args[1], 1, from, call_stack);

    List& list = args[0].Get<List>();
    Int index = args[1].Get<Int>();

    if (index < 0 || index >= list->size()) {
        ThrowError<lang_exceptions::IndexOutOfRangeError>(
            std::move(from), call_stack, index, list->size()
        );
    }

    list->Remove(index);
    return list;
}

Value Sort(std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<List>(args[0], 0, from, call_stack);
    List& list = args[0].Get<List>();
    list->Sort();
    return list;
}

Value Set(std::vector<Value>& args, Token from, const CallStack& call_stack) {
    AssertType<List>(args[0], 0, from, call_stack);
    AssertType<Int>(args[1], 1, from, call_stack);

    List& list = args[0].Get<List>();
    Int index = args[1].Get<Int>();

    if (index < 0 || index >= list->size()) {
        ThrowError<lang_exceptions::IndexOutOfRangeError>(
            std::move(from), call_stack, index, list->size()
        );
    }

    list->At(index) = args[2].GetCopy();
    return list;
}

} // namespace lists
    
} // namespace stdlib
    
} // namespace itmoscript
