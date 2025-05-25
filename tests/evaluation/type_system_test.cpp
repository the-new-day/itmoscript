#include "evaluation_test.hpp"

using TS = itmoscript::TypeSystem;

void RegisterConversions(itmoscript::TypeSystem& type_system) {
    type_system.RegisterConversion<itmoscript::Int, itmoscript::Float>([](const itmoscript::Int& value) { 
        return static_cast<itmoscript::Float>(value);
    });

    type_system.RegisterConversion<itmoscript::Float, itmoscript::Int>([](const itmoscript::Float& value) { 
        return static_cast<itmoscript::Int>(value);
    });
}

TEST(EvaluationTestSuite, TypeConversionTest) {
    itmoscript::TypeSystem type_system;
    RegisterConversions(type_system);

    auto float_type = TS::GetValueType<itmoscript::Float>();
    auto int_type = TS::GetValueType<itmoscript::Int>();
    auto bool_type = TS::GetValueType<itmoscript::Bool>();

    // <converted, to, expected>
    std::vector<std::tuple<IsValue, IsValueType, IsValue>> expressions = {
        {*type_system.TryConvert(IsValue(10), float_type), float_type, IsValue(10.0)},
        {*type_system.TryConvert(IsValue(10.4), int_type), int_type, IsValue(10)},
        {*type_system.TryConvert(IsValue(10.4), bool_type), bool_type, true},
        {*type_system.TryConvert(IsValue(0), bool_type), bool_type, false},
        {*type_system.TryConvert(IsValue(std::string{"aaa"}), bool_type), bool_type, true},
        {*type_system.TryConvert(IsValue(std::string{""}), bool_type), bool_type, false},
    };

    for (const auto& [src, type, converted] : expressions) {
        ASSERT_EQ(converted.GetType(), type);
        ASSERT_EQ(src, converted);
    }
}