#pragma once
#include <string>
#include "../../spirit/heart.hpp"

/**
 * @brief The base class for all expression types in the AST.
 *
 * Expressions are units that produce values — such as literals, variables,
 * or operations. All expression types derive from this base.
 *
 * "The unfolding of Your words gives light; it gives understanding to the simple."
 * — Psalm 119:130
 */
class Expr
{
public:

    /**
     * @brief Evaluates the expression and returns a Value.
     *
     * "But the one who looks into the perfect law... will be blessed." — James 1:25
     */
    virtual Value evaluate(Heart *heart) = 0;

    virtual ~Expr() = default;

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const { return "Expr"; }
};
