#pragma once

#include "expr.hpp"
#include <memory>
#include "../../spirit/value.hpp"

/**
 * @brief Represents a parenthesized expression: (a + b)
 *
 * Groups an inner expression to give it precedence during evaluation,
 * much like how we focus our thoughts in prayer or meditation to discern meaning.
 *
 * "But when you pray, go into your room, close the door and pray to your Father, who is unseen.
 * Then your Father, who sees what is done in secret, will reward you."
 * — Matthew 6:6
 *
 * @param expression The inner expression being grouped and given precedence.
 */
class GroupingExpr : public Expr
{
public:
    std::unique_ptr<Expr> expression;

    /**
     * @brief Constructs a GroupingExpr with a given inner expression.
     *
     * Initializes the grouped expression, giving intentional focus and
     * structural priority to what is within the parentheses.
     *
     * "I meditate on your precepts and consider your ways." — Psalm 119:15
     *
     * Grouping in code is like meditation — it's purposeful focus.
     *
     * @param expression The expression to be grouped and evaluated with precedence.
     */
    explicit GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    Value evaluate(Heart *heart) const override
    {
        return expression->evaluate(heart);
    }

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const override { return "GroupingExpr(" + expression->toString() + ")"; }

    Value accept(ExprVisitor &visitor) const override;
};
