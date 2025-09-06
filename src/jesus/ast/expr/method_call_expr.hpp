#pragma once

#include "expr.hpp"

class Method; // Forward declaration

class MethodCallExpr : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::shared_ptr<Method> method;

    MethodCallExpr(std::unique_ptr<Expr> object, std::shared_ptr<Method> method)
        : object(std::move(object)), method(std::move(method))
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
};
