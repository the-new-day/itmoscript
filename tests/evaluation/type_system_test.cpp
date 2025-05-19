#include "evaluation_test.hpp"

TEST(EvaluationTestSuite, TypeConversionTest) {
    ItmoScript::TypeSystem type_system;

    std::vector<std::tuple<Value, std::type_index, Value>> expressions{
        {*type_system.TryConvert(Value(10), typeid(ItmoScript::Float)), typeid(ItmoScript::Float), Value(10.0)},
        {*type_system.TryConvert(Value(10.4), typeid(ItmoScript::Int)), typeid(ItmoScript::Int), Value(10)},
    };

    for (const auto& [src, type, converted] : expressions) {
        ASSERT_EQ(converted.GetTypeIndex(), type);
        ASSERT_EQ(src, converted);
    }
}