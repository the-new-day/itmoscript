#include "evaluation_units_test.hpp"

using TS = itmoscript::TypeSystem;

void RegisterConversions(itmoscript::TypeSystem& type_system) {
    type_system.RegisterConversion<itmoscript::Int, itmoscript::Float>([](const itmoscript::Int& value) { 
        return static_cast<itmoscript::Float>(value);
    });

    type_system.RegisterConversion<itmoscript::Float, itmoscript::Int>([](const itmoscript::Float& value) { 
        return static_cast<itmoscript::Int>(value);
    });
}

TEST(EvaluationTypesTestSuite, ExplicitConversionTest) {
    itmoscript::TypeSystem type_system;
    RegisterConversions(type_system);

    auto float_type = TS::GetValueType<itmoscript::Float>();
    auto int_type = TS::GetValueType<itmoscript::Int>();
    auto bool_type = TS::GetValueType<itmoscript::Bool>();
    auto string_type = TS::GetValueType<itmoscript::String>();

    // <value, requested type, expected>
    std::vector<std::tuple<IsValue, IsValueType, std::optional<IsValue>>> expressions = {
        {IsValue{1}, float_type, IsValue{1.0}},
        {IsValue{1.5}, int_type, IsValue{1}},
        {IsValue{1.5}, float_type, IsValue{1.5}},
        {IsValue{1}, int_type, IsValue{1}},
        
        {IsValue{true}, string_type, std::nullopt},
    };

    for (const auto& [value, requested_type, expected] : expressions) {
        ASSERT_EQ(type_system.TryConvert(value, requested_type), expected)
            << "value: " << value << "; requested type: " << itmoscript::GetTypeName(value.type()) 
            << "; expected: " 
            << (expected.has_value() ? *expected : IsValue{std::make_shared<std::string>("nullopt")});
    }
}

TEST(EvaluationTypesTestSuite, CommonTypeTest) {
    itmoscript::TypeSystem type_system;
    RegisterConversions(type_system);

    auto float_type = TS::GetValueType<itmoscript::Float>();
    auto int_type = TS::GetValueType<itmoscript::Int>();
    auto bool_type = TS::GetValueType<itmoscript::Bool>();
    auto string_type = TS::GetValueType<itmoscript::String>();
    auto function_type = TS::GetValueType<itmoscript::Function>();
    auto list_type = TS::GetValueType<itmoscript::List>();

    // <a, b, expected common type>
    std::vector<std::tuple<IsValueType, IsValueType, std::optional<IsValueType>>> expressions = {
        {int_type, float_type, float_type},
        {float_type, int_type, float_type},
        {int_type, int_type, int_type},
        {float_type, float_type, float_type},

        {string_type, function_type, std::nullopt},
        {int_type, string_type, std::nullopt},
        {float_type, function_type, std::nullopt},
        {bool_type, function_type, std::nullopt},
        {string_type, list_type, std::nullopt},
    };

    for (const auto& [a, b, expected] : expressions) {
        ASSERT_EQ(type_system.FindCommonType(a, b), expected)
            << "a: " << itmoscript::GetTypeName(a) 
            << "; b: " << itmoscript::GetTypeName(b) 
            << "; expected: " 
            << (expected.has_value() ? itmoscript::GetTypeName(*expected) : "nullopt");
    }
}
