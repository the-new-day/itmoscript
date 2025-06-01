#include "OperatorRegistry.hpp"

namespace itmoscript {

void OperatorRegistry::RegisterCommutativeOperatorForAllPairsOfTypes(TokenType oper, BinaryHandler handler) {
    RegisterCommutativeOperatorForAllTypes<NullType>(oper, handler);
    RegisterCommutativeOperatorForAllTypes<Int>(oper, handler);
    RegisterCommutativeOperatorForAllTypes<Float>(oper, handler);
    RegisterCommutativeOperatorForAllTypes<String>(oper, handler);
    RegisterCommutativeOperatorForAllTypes<Bool>(oper, handler);
    RegisterCommutativeOperatorForAllTypes<Function>(oper, handler);
    RegisterCommutativeOperatorForAllTypes<List>(oper, handler);
}

void OperatorRegistry::RegisterUnaryOperatorForAllTypes(TokenType oper, UnaryHandler handler) {
    RegisterUnaryOper<NullType>(oper, handler);
    RegisterUnaryOper<Int>(oper, handler);
    RegisterUnaryOper<Float>(oper, handler);
    RegisterUnaryOper<String>(oper, handler);
    RegisterUnaryOper<Bool>(oper, handler);
    RegisterUnaryOper<Function>(oper, handler);
    RegisterUnaryOper<List>(oper, handler);
}

std::optional<OperatorRegistry::BinaryHandler>
OperatorRegistry::FindExactHandler(TokenType oper, ValueType left, ValueType right) {
    auto key = std::make_pair(left, right);
    if (binary_ops_.contains(oper) && binary_ops_[oper].contains(key)) {
        return binary_ops_[oper][key];
    }

    return std::nullopt;
}

std::optional<OperatorRegistry::UnaryHandler>
OperatorRegistry::FindExactHandler(TokenType oper, ValueType right) {
    if (unary_ops_.contains(oper) && unary_ops_[oper].contains(right)) {
        return unary_ops_[oper][right];
    }

    return std::nullopt;
}

} // namespace itmoscript
