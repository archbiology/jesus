#pragma once

#include "expr.hpp"

class Interpreter;

REGISTER_FOR_UML(
    AssignableExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"}));

/**
 * @brief Represents variable updates: (target) = expression
 *
 * Where target can be:
 *  variable        → x = 10
 *  indexed access  → list[0] = 10
 *  attribute       → person.name = "Adam"
 */
class AssignableExpr : public Expr
{
public:
    virtual void assign(Interpreter &interpreter, const Value &value) const = 0;
};
