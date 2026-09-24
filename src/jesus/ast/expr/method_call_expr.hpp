#pragma once

#include "expr.hpp"

class IMethod; // Forward declaration
class Interpreter; // Forward declaration

REGISTER_FOR_UML(
    MethodCallExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"object", "method", "args", "argIndices"}));

class MethodCallExpr : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::shared_ptr<IMethod> method;
    std::vector<std::unique_ptr<Expr>> args;
    Interpreter *interpreter = nullptr;

    /**
     * @brief Parameter index filled by each element of `args`.
     *
     * When a call omits parameters that have default values, `args` holds
     * only the provided values; the i-th entry of `args` fills the
     * paramIdx = argIndices[i] parameter. The defaults are substituted at
     * runtime. Empty when no defaults are involved (`args` is then the
     * full, argument-in-declaration-order list).
     */
    std::vector<size_t> argIndices;

    MethodCallExpr(
        std::unique_ptr<Expr> object,
        std::shared_ptr<IMethod> method,
        std::vector<std::unique_ptr<Expr>> args,
        Interpreter *interpreter_,
        std::vector<size_t> argIndices = {})
        : object(std::move(object)), method(std::move(method)), args(std::move(args)), interpreter(interpreter_),
          argIndices(std::move(argIndices))
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
