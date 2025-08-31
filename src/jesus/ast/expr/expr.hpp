#pragma once
#include <string>
#include "../../spirit/heart.hpp"

class ExprVisitor; // Forward declaration

/**
 * @brief The base class for all expression types in the AST.
 *
 * Expressions are units that produce values — such as literals, variables,
 * or operations. All expression types derive from this base.
 *
 * Expressions represent values or computations (variables, literals, arithmetic, function calls).
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
    virtual Value evaluate(std::shared_ptr<Heart> heart) const = 0;

    virtual Value accept(ExprVisitor &visitor) const = 0;

    virtual ~Expr() = default;

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const { return "Expr"; }
};
