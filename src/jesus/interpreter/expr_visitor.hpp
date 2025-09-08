
#pragma once

#include "../ast/expr/binary_expr.hpp"
#include "../ast/expr/conditional_expr.hpp"
#include "../ast/expr/unary_expr.hpp"
#include "../ast/expr/literal_expr.hpp"
#include "../ast/expr/variable_expr.hpp"
#include "../ast/expr/grouping_expr.hpp"
#include "../ast/expr/ask_expr.hpp"
#include "../ast/expr/create_instance_expr.hpp"
#include "../ast/expr/get_attr_expr.hpp"
#include "../ast/expr/method_call_expr.hpp"

/**
 * @brief Interface for visiting and evaluating expression nodes in the AST.
 *
 * The ExprVisitor defines a set of methods that must be implemented
 * by any class that wants to interpret expression types.
 *
 * This allows the interpreter (or a JIT compiler, debugger, etc.) to simplify
 * its evaluate method by calling expression->accept(visitor) instead of
 * manually checking the expression type with conditionals or dynamic casts.
 *
 * Usage example:
 *
 *     class Interpreter : public ExprVisitor {
 *         Value visitBinary(const BinaryExpr& expr) override;
 *         Value visitLiteral(const LiteralExpr& expr) override;
 *         // ...
 *     };
 *
 *     std::unique_ptr<Expr> expression = std::make_unique<BinaryExpr>(...)
 *     Interpreter interpreter;
 *     Value result = expression->accept(interpreter);
 *
 * Currently being called on the `evaluate` method of Interpreter:
 *     Value Interpreter::evaluate(const std::unique_ptr<Expr> &expr) {
 *       return expr->accept(*this);
 *     }
 *
 * ---
 * "Do not forget to show hospitality to strangers, for by so doing
 * some people have shown hospitality to angels without knowing it."
 * — Hebrews 13:2
 */
class ExprVisitor
{
public:
    virtual Value visitBinary(const BinaryExpr &expr) = 0;
    virtual Value visitLiteral(const LiteralExpr &expr) = 0;
    virtual Value visitGrouping(const GroupingExpr &expr) = 0;
    virtual Value visitUnary(const UnaryExpr &expr) = 0;
    virtual Value visitConditional(const ConditionalExpr &expr) = 0;
    virtual Value visitVariable(const VariableExpr &expr) = 0;
    virtual Value visitAsk(const AskExpr &expr) = 0;
    virtual Value visitCreateInstanceExpr(const CreateInstanceExpr &expr) = 0;
    virtual Value visitGetAttribute(const GetAttributeExpr &expr) = 0;
    virtual Value visitMethodCallExpr(const MethodCallExpr &expr) = 0;

    virtual ~ExprVisitor() = default;
};
