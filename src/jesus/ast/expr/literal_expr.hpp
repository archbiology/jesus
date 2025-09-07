#pragma once

#include "expr.hpp"

/**
 * @brief Represents a literal value like a number, string, or boolean.
 *
 * A literal holds a fixed, constant truth — a value not derived from computation,
 * but directly declared. In programming as in life, truth brings clarity.
 *
 * "The sum of your word is truth, and every one of your righteous rules endures forever."
 *  — Psalm 119:160
 */
class LiteralExpr : public Expr
{
public:
    Value value;
    std::shared_ptr<CreationType> type;

    /**
     * @brief Construct a new LiteralExpr with a given value.
     *
     * Wraps a raw, unchanging value in an expression node.
     * Literal expressions represent the foundational truths within a program.
     *
     * "Every word of God proves true; he is a shield to those who take refuge in him."
     * — Proverbs 30:5
     *
     * @param value  The literal value (e.g., 7, "Jesus", true, "Wisdom").
     */
    explicit LiteralExpr(Value value, std::shared_ptr<CreationType> type)
        : value(value), type(std::move(type)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return value;
    }

    Value accept(ExprVisitor &visitor) const override;

    /**
     * @brief Get the return type of the expression, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const override { return "LiteralExpr(" + value.toString() + ")"; }
};
