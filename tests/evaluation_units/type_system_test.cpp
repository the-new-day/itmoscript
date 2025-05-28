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
            << "value: " << value << "; requested type: " << itmoscript::kValueTypeNames.at(value.type()) 
            << "; expected: " << (expected.has_value() ? *expected : IsValue{std::string{"nullopt"}});
    }
}

TEST(EvaluationTypesTestSuite, CommonTypeTest) {
    itmoscript::TypeSystem type_system;
    RegisterConversions(type_system);

    auto float_type = TS::GetValueType<itmoscript::Float>();
    auto int_type = TS::GetValueType<itmoscript::Int>();
    auto bool_type = TS::GetValueType<itmoscript::Bool>();

    // <a, b, expected common type>
    std::vector<std::tuple<IsValue, IsValue, std::optional<IsValueType>>> expressions = {
        {IsValue{1}, IsValue{2.2}, float_type},
        {IsValue{2.2}, IsValue{1}, float_type},
        {IsValue{2}, IsValue{1}, int_type},
        {IsValue{2.2}, IsValue{1.3}, float_type},

        {IsValue{itmoscript::String{"aaa"}}, IsValue{itmoscript::Function{}}, std::nullopt},
        {IsValue{1}, IsValue{itmoscript::String{"aaa"}}, std::nullopt},
        {IsValue{1.5}, IsValue{itmoscript::Function{}}, std::nullopt},
        {IsValue{true}, IsValue{itmoscript::Function{}}, std::nullopt},
        // {IsValue{itmoscript::String{"aaa"}}, IsValue{itmoscript::List{}}, std::nullopt},
    };

    for (const auto& [a, b, expected] : expressions) {
        ASSERT_EQ(type_system.FindCommonType(a.type(), b.type()), expected)
            << "a: " << a << "; b: " << b << "; expected: " 
                << (expected.has_value() ? itmoscript::kValueTypeNames.at(*expected) : IsValue{std::string{"nullopt"}});
    }
}
