#pragma once

#include "ast_node.hpp"
#include "../spirit/heart.hpp"
#include "../ast/expr/expr.hpp"
#include <string>
#include <memory>

/**
 * @brief Wraps any single expression node.
 *
 * This node delegates all evaluation to its inner expression.
 *
 * Useful for grouping or simplifying AST node handling.
 *
 * 📖 "The unfolding of your words gives light; it gives understanding to the simple."
 * — Psalm 119:130
 */
struct ExpressionNode : public ASTNode
{
    std::unique_ptr<Expr> expr;

    explicit ExpressionNode(std::unique_ptr<Expr> expr)
    : expr(std::move(expr)) {}

    void execute(Heart *heart) override
    {
        Value value = evaluate(heart);

        if (! value.IS_FORMLESS)
        {
            std::cout << value.toString() << std::endl;
        }
    }

    Value evaluate(Heart *heart) override
    {
        if (!expr)
            return Value::formless();

        return expr->evaluate(heart);
    }

    std::string toString() const override
    {
        std::string str = "ExpressionNode";

        if (expr)
            str += "(" + expr->toString() + ")";

        return str;
    }
};

