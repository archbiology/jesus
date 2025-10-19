#pragma once

#include "expr.hpp"

/**
 * @brief Represents an expression that wraps a child instance when assigned to a parent type.
 *
 * This expression is automatically created when a value of a descendant type
 * (e.g., a `Son`) is assigned to a variable of an ancestor type (e.g., a `Parent`).
 *
 * Instead of converting or copying, it wraps the original value while preserving
 * its true identity and behavior — allowing the system to recognize that the
 * instance “is” the parent, yet still “acts” as the child when appropriate.
 *
 * This models classical polymorphism: a child may walk in the name of its father,
 * but remains distinct in nature and purpose.
 *
 * “If you were Abraham’s children,” said Jesus, “then you would do what Abraham did.
 * But you seek to kill Me... You belong to your father, the devil.” — John 8:39-44
 */
class PolymorphismExpr : public Expr
{
public:
    std::unique_ptr<Expr> valueExpr; // the child instance

    PolymorphismExpr(std::unique_ptr<Expr> val)
        : valueExpr(std::move(val)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return valueExpr->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override
    {
        return valueExpr->accept(visitor);
    };

    bool canEvaluateAtParseTime() const override
    {
        return valueExpr->canEvaluateAtParseTime();
    }

    /**
     * @brief Get the method return type, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override
    {
        return valueExpr->getReturnType(ctx);
    }

    virtual std::string toString() const override { return "PolymorphismExpr()"; }
};
