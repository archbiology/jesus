#pragma once

#include "expr.hpp"
#include <string>

/**
 * @brief Represents a variable reference like "age" or "name".
 *
 * A variable expression retrieves the value associated with a name.
 * Just as names in the Bible often reflect identity and calling,
 * this expression looks up what has been stored — something defined,
 * declared, and meaningful.
 *
 * "I have called you by your name; you are mine." — Isaiah 43:1
 */
class VariableExpr : public Expr
{
public:
    std::string name;

    /**
     * @brief Constructs a VariableExpr with a given name.
     *
     * This represents a reference to a previously defined value.
     * The variable’s name is its key — and retrieving it brings forward
     * what it was purposed to carry.
     *
     * "Before I formed you in the womb I knew you,
     * and before you were born I consecrated you."
     * — Jeremiah 1:5
     *
     * @param name The name of the variable being referenced (e.g., prophet).
     */
    explicit VariableExpr(std::string name)
        : name(name) {}

    Value evaluate(Heart *heart) const override
    {
        return heart->getVar(name);
    }

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const override { return "VariableExpr(" + name + ")"; }

    Value accept(ExprVisitor &visitor) const override;
};
