#pragma once
#include <string>
#include "../../spirit/heart.hpp"

class ExprVisitor;   // Forward declaration
class ParserContext; // Forward declaration

enum class ExprKind
{
    Literal,
    Other
};

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
    const ExprKind kind;

public:
    explicit Expr(ExprKind kind = ExprKind::Other) : kind(kind) {}
    /**
     * @brief Evaluates the expression and returns a Value.
     *
     * "But the one who looks into the perfect law... will be blessed." — James 1:25
     */
    virtual Value evaluate(std::shared_ptr<Heart> heart) const = 0;

    virtual Value accept(ExprVisitor &visitor) const = 0;

    virtual bool canEvaluateAtParseTime() const
    {
        return kind == ExprKind::Literal;
    }

    /**
     * @brief Get the return type of the expression, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    virtual std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const = 0;

    virtual ~Expr() = default;

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const { return "Expr"; }
};
