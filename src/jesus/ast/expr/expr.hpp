#pragma once

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
    virtual ~Expr() = default;
};
