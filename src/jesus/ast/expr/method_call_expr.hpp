#pragma once

#include "expr.hpp"

class Method; // Forward declaration

class MethodCallExpr : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::shared_ptr<Method> method;
    std::vector<std::unique_ptr<Expr>> args;

    MethodCallExpr(
        std::unique_ptr<Expr> object,
        std::shared_ptr<Method> method,
        std::vector<std::unique_ptr<Expr>> args)
        : object(std::move(object)), method(std::move(method)), args(std::move(args))
    {
        if (this->method == nullptr)
        {
            throw std::runtime_error("Method cannot be empty!");
        }
    }

    /**
     * @brief Return the 'object' that contains the 'method' to be called
     */
    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return object->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override;

    /**
     * @brief Get the method return type, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;
};
