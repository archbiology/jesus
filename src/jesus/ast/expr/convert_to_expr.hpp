#pragma once

#include "expr.hpp"

/**
 * @brief Represents an explicit conversion from one type to another using the `to` keyword.
 *
 * This expression models type conversion in the Jesus language, allowing values
 * to be transformed or safely cast into another known type:
 *
 *     son = abraham to Son
 *
 * If the conversion is valid — such as a parent instance being interpreted
 * as a specific child — the resulting expression retains its identity
 * while adapting to the requested type.
 *
 * When the conversion is impossible, the result is `null`, reflecting
 * that not all transformations are true or meaningful.
 *
 * “If you were Abraham’s children, you would do what Abraham did.
 * But now you seek to kill Me... You belong to your father, the devil.” — John 8:39–44
 */
class ConvertToExpr : public Expr
{
public:
    std::unique_ptr<Expr> valueExpr;          // the expression to cast
    std::shared_ptr<CreationType> targetType; // the type we want to cast to

    ConvertToExpr(std::unique_ptr<Expr> value, std::shared_ptr<CreationType> type)
        : valueExpr(std::move(value)), targetType(std::move(type)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override

    {
        Value val = valueExpr->evaluate(heart);

        if (!val.IS_INSTANCE)
            return Value::formless();

        return valueExpr->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override
    {
        return valueExpr->accept(visitor);
    }

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override
    {
        return targetType;
    }

    virtual std::string toString() const override { return "ToExpr()"; }
};
