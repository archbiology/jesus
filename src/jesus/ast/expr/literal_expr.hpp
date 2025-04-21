#pragma once

#include "expr.hpp"
#include <variant>
#include <string>

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
    std::variant<std::string, double, bool> value;

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
    explicit LiteralExpr(const std::variant<std::string, double, bool> &value)
        : value(value) {}
};
