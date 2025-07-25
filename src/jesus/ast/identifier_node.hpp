#pragma once

#include "../ast/expr/expr.hpp"
#include "../spirit/heart.hpp"
#include <string>

/**
 * @brief AST node representing an identifier (e.g., variable name).
 *
 * The IdentifierNode holds the name of a symbol used in the program,
 * such as a variable, function, or other named entity. In a complete
 * interpreter or compiler, this node would typically resolve the identifier
 * from a symbol table or environment during execution.
 *
 * Currently, the `execute()` function is a placeholder and does nothing.
 *
 * "The Lord knows those who are his." — 2 Timothy 2:19
 * In the same way, each identifier must be identified within the language.
 */
struct IdentifierNode : public Expr
{
    std::string name;

    IdentifierNode(const std::string &name) : name(name) {}

    Value evaluate(Heart *heart) override
    {
        if (heart)
        {
            return heart->getVar(name);
        }

        return Value::formless();
    }

    std::string toString() const override { return "IdentifierNode(" + name + ")"; }
};
