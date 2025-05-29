#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <stack>

#include "utils.hpp"

#include "ast/AstVisitor.hpp"
#include "ast/AST.hpp"

#include "objects/Value.hpp"
#include "objects/Function.hpp"
#include "objects/List.hpp"

#include "evaluation/TypeSystem.hpp"
#include "evaluation/OperatorRegistry.hpp"
#include "evaluation/Environment.hpp"
#include "evaluation/CallFrame.hpp"

#include "exceptions/ZeroDivisionError.hpp"
#include "exceptions/SequenceMultiplicationError.hpp"

namespace itmoscript {

/**
 * @class Evaluator
 * @brief Walks through the AST and evaluates every node. Controls the flow of evaluation,
 * block scopes. Holds registered identifiers and their values.
 * 
 * @details Implements the AstVisitor interface, making walking through the tree easy.
 * All Visit() methods are private, the Node class is a friend of AstVisitor.
 */
class Evaluator : public ast::AstVisitor {
public:
    /**
     * @brief Consturcts the evaluator.
     * Registers all operators and sets up the environment stack, initializing the global scope.
     */
    Evaluator();

    /**
     * @brief Walks through the AST from given root.
     * Clears the call stack before execution.
     * 
     * All AST nodes, which control flow reaches, are evaluated.
     */
    void Interpret(ast::Program& root);

    const Value& GetLastEvaluatedValue() const;

private:
    Token current_token_;

    TypeSystem type_system_;
    OperatorRegistry operator_registry_;

    bool inside_loop_ = false;

    enum class ControlFlowState {
        kNormal,
        kReturn,
        kBreak,
        kContinue,
    };

    struct ExecResult {
        Value value;
        ControlFlowState control = ControlFlowState::kNormal;
    };

    CallStack call_stack_;
    std::stack<std::shared_ptr<Environment>> env_stack_;

    ExecResult last_exec_result_;

    Environment& env();

    /**
     * @brief "Enters" new scope. Pushes new environment to the env_stack_,
     * with parent link to the current scope.
     */
    void PushEnv();

    /**
     * @brief "Leaves" the current scope. Erases all identifiers registed inside of it.
     */
    void PopEnv();

    /**
     * @brief Evaluates the given node by calling node.Accept(*this).
     * Also sets the current_token_.
     */
    const ExecResult& Eval(ast::Node& node);

    /**
     * @brief Calls the matching handler for the operator with given value if it exists.
     * @return Evaluated value if the handler was found, std::nullopt otherwise.
     */
    std::optional<Value> HandleUnaryOper(TokenType oper, const Value& right);

    /**
     * @brief Calls the matching handler for the operator with given values if it exists.
     * @return Evaluated value if the handler was found, std::nullopt otherwise.
     */
    std::optional<Value> HandleBinaryOper(TokenType oper, const Value& left, const Value& right);

    /**
     * @brief Checks if the given identifier is registered either in the current scope or in any
     * of the outer scopes. If it is, returns const ref to it's value.
     * @throw UndefinedNameError If the identifier is not found in all of the scope chain.
     */
    const Value& ResolveIdentifier(const ast::Identifier& ident);

    /**
     * @brief Assigns the identifier with the given value.
     * If the identifier exists in the current scope or in any outer scope, changes it's value.
     * 
     * Otherwise, registers the new identifier with the given value.
     */
    void AssignIdentifier(const ast::Identifier& ident, Value value);

    /**
     * @brief Calls the function with given arguments.
     * @param name Name of the function for the stacktrace. Used only for printing the stacktrace.
     * @param func Function object to call. Must contains valid parameters and body fields.
     * @param args Arguments to pass to the function
     * 
     * @details When the control flow enters the function, it creates it's local scope with parameters
     * set to the corresponing values passed to the function.
     * So, local function parameters "shadow" parameters from the outer scope.
     */
    Value CallFunction(std::string name, const Function& func, std::vector<Value>& args);

    /**
     * @brief Returns the identifier associated with the function if it's named,
     * "<anonymous function>" otherwise.
     */
    std::string GetFunctionName(const std::optional<std::string>& name);

    void EvalSliceIndexExpression(ast::IndexOperatorExpression& expr);

    template<NumericValueType T>
    void RegisterCommonAriphmeticOps();

    template<NumericValueType T>
    void RegisterStringMultiplication();

    template<NumericValueType T>
    void RegisterListMultiplication();
    
    void RegisterTypeConversions();
    void RegisterAriphmeticOps();
    void RegisterUnaryOps();
    void RegisterComparisonOps();
    void RegisterStringOps();
    void RegisterLogicalOps();
    void RegisterListOps();

    /**
     * @brief Throws RuntimeError's inheritant exception with given type and arguments.
     * Also includes current token and current call stack to the exception constructor.
     */
    template<typename ErrorType, typename ...Args>
    void ThrowRuntimeError(Args&&... args) const noexcept(false);

    /**
     * @brief Throws RuntimeError's inheritant exception with given type and arguments.
     * Also includes the passed token and current call stack to the exception constructor.
     */
    template<typename ErrorType, typename ...Args>
    void ThrowRuntimeError(Token token, Args&&... args) const noexcept(false);

    // Visitor implementation

    void Visit(ast::Program&) override;
    void Visit(ast::ExpressionStatement&) override;
    void Visit(ast::PrefixExpression&) override;
    void Visit(ast::InfixExpression&) override;
    void Visit(ast::IndexOperatorExpression&) override;

    void Visit(ast::Identifier&) override;
    void Visit(ast::IntegerLiteral&) override;
    void Visit(ast::BooleanLiteral&) override;
    void Visit(ast::NullTypeLiteral&) override;
    void Visit(ast::FloatLiteral&) override;
    void Visit(ast::StringLiteral&) override;
    void Visit(ast::FunctionLiteral&) override;
    void Visit(ast::ListLiteral&) override;

    void Visit(ast::IfExpression&) override;
    void Visit(ast::BlockStatement&) override;
    void Visit(ast::AssignStatement&) override;
    void Visit(ast::OperatorAssignStatement&) override;
    void Visit(ast::CallExpression&) override;
    void Visit(ast::ReturnStatement&) override;

    void Visit(ast::WhileStatement&) override;
    void Visit(ast::ForStatement&) override;
    void Visit(ast::BreakStatement&) override;
    void Visit(ast::ContinueStatement&) override;

    template<typename InnerType>
    static std::shared_ptr<InnerType> CreateHeavyValue(InnerType& val) {
        return std::make_shared<InnerType>(std::move(val));
    }

    static String CreateString(std::string val) {
        return CreateHeavyValue(val);
    }

    static List CreateList(ListObject val) {
        return CreateHeavyValue(val);
    }
};

template<NumericValueType T>
void Evaluator::RegisterCommonAriphmeticOps() {
    operator_registry_.RegisterBinaryOper<T, T>(TokenType::kPlus, 
        [](const Value& left, const Value& right) { return left.Get<T>() + right.Get<T>(); });

    operator_registry_.RegisterBinaryOper<T, T>(TokenType::kMinus, 
        [](const Value& left, const Value& right) { return left.Get<T>() - right.Get<T>(); });

    operator_registry_.RegisterBinaryOper<T, T>(TokenType::kAsterisk, 
        [](const Value& left, const Value& right) { return left.Get<T>() * right.Get<T>(); });

    operator_registry_.RegisterBinaryOper<T, T>(TokenType::kSlash, [this](const Value& left, const Value& right) { 
        if (right.Get<T>() == 0) {
            ThrowRuntimeError<lang_exceptions::ZeroDivisionError>();
        }
        
        return left.Get<T>() / right.Get<T>();
    });

    operator_registry_.RegisterUnaryOper<T>(TokenType::kPlus, [](const Value& right) { return right; });
    operator_registry_.RegisterUnaryOper<T>(TokenType::kMinus, [](const Value& right) { return -right.Get<T>(); });
}

template<NumericValueType T>
void Evaluator::RegisterStringMultiplication() {
    operator_registry_.RegisterCommutativeOperator<String, T>(
        TokenType::kAsterisk, 
        [this](const Value& left, const Value& right) -> Value {
        const String& str = left.IsOfType<String>() ? left.Get<String>() : right.Get<String>();
        T number = left.IsOfType<String>() ? right.Get<T>() : left.Get<T>();

        std::optional<std::string> result = utils::MultiplyStr(*str, number);
        if (!result) {
            ThrowRuntimeError<lang_exceptions::SequenceMultiplicationError>(
                std::format(
                    "cannot multiply string \"{}\" by negative value {}",
                    *str,
                    number
                )
            );
        }

        return CreateString(std::move(*result));
    });
}

template<NumericValueType T>
void Evaluator::RegisterListMultiplication() {
    operator_registry_.RegisterCommutativeOperator<List, T>(
        TokenType::kAsterisk, 
        [this](const Value& left, const Value& right) -> Value {
        const List& list = left.IsOfType<List>() ? left.Get<List>() : right.Get<List>();
        T number = left.IsOfType<List>() ? right.Get<T>() : left.Get<T>();

        std::optional<std::vector<Value>> result = utils::MultiplyVec<Value>(list->data(), number);

        if (!result) {
            ThrowRuntimeError<lang_exceptions::SequenceMultiplicationError>(
                std::format(
                    "cannot multiply list by negative value {}",
                    number
                )
            );
        }

        return CreateList(std::move(*result));
    });
}

template<typename ErrorType, typename ...Args>
void Evaluator::ThrowRuntimeError(Token token, Args&&... args) const {
    throw ErrorType{token, call_stack_, std::forward<Args>(args)...};
}

template<typename ErrorType, typename ...Args>
void Evaluator::ThrowRuntimeError(Args&&... args) const {
    throw ErrorType{current_token_, call_stack_, std::forward<Args>(args)...};
}

} // namespace itmoscript
