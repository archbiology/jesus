#pragma once

#include "expr.hpp"

class IMethod; // Forward declaration
class Interpreter; // Forward declaration

REGISTER_FOR_UML(
    MethodCallExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"object", "method", "args"}));

class MethodCallExpr : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::shared_ptr<IMethod> method;
    std::vector<std::unique_ptr<Expr>> args;
    Interpreter *interpreter = nullptr;

    MethodCallExpr(
        std::unique_ptr<Expr> object,
        std::shared_ptr<IMethod> method,
        std::vector<std::unique_ptr<Expr>> args, Interpreter *interpreter_)
        : object(std::move(object)), method(std::move(method)), args(std::move(args)), interpreter(interpreter_)
    {
        if (this->method == nullptr)
        {
            throw std::runtime_error("Method cannot be empty!");
        }
    }

    void validate(ParserContext &ctx) const override;

    /**
     * @brief Calls the method and returns its value: return object.method(args)
     */
    Value evaluate(std::shared_ptr<Heart> heart) const override;

    Value accept(ExprVisitor &visitor) const override;

    /**
     * @brief Get the method return type, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

private:
    bool isArgumentAssignable(
        const std::shared_ptr<CreationType> &paramType,
        const std::shared_ptr<CreationType> &argType) const;
};
