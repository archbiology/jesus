#pragma once

#include "expr.hpp"
#include "lexer/token.hpp"
#include <memory>

/**
 * @brief Represents a binary operation like `a + b` or `a and b`.
 *
 * A binary expression combines two operands using an operator. It is
 * the core of many logical and arithmetic operations within the language.
 *
 * "Two are better than one, because they have a good reward for their labor."
 * — Ecclesiastes 4:9
 */
class BinaryExpr : public Expr
{
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    /**
     * @brief @brief Constructs a BinaryExpr with a left operand, an operator, and a right operand.
     *
     * Thse expression evaluate the left and right sides and apply the operator.
     *
     * "Though one may be overpowered, two can defend themselves. A cord of three strands is not quickly broken."
     * — Ecclesiastes 4:12
     *
     * @param left  The left-hand side of the binary expression (e.g., an operand or sub-expression).
     * @param op    The operator token that defines the operation (e.g., '+', 'and', '>=').
     * @param right The right-hand side of the binary expression.
     */
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
};
