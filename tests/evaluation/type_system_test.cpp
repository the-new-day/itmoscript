#include "evaluation_test.hpp"

using TS = itmoscript::TypeSystem;

TEST(EvaluationTestSuite, TypeConversionTest) {
    itmoscript::TypeSystem type_system;
    auto float_type = TS::GetValueType<itmoscript::Float>();
    auto int_type = TS::GetValueType<itmoscript::Int>();

    // <converted, to, expected>
    std::vector<std::tuple<IsValue, IsValueType, IsValue>> expressions{
        {*type_system.TryConvert(IsValue(10), float_type), float_type, IsValue(10.0)},
        {*type_system.TryConvert(IsValue(10.4), int_type), int_type, IsValue(10)},
    };

    for (const auto& [src, type, converted] : expressions) {
        ASSERT_EQ(converted.GetType(), type);
        ASSERT_EQ(src, converted);
    }
}