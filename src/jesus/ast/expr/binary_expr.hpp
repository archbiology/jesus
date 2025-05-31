#pragma once

#include "expr.hpp"
#include "../../lexer/token.hpp"
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

    Value evaluate(Heart *heart) override
    {

        Value leftVal = left->evaluate(heart);
        Value rightVal = right->evaluate(heart);

        if (op.type == TokenType::EQUAL_EQUAL)
        {
            return Value(leftVal == rightVal);
        }

        if (op.type == TokenType::PLUS)
        {
            return leftVal + rightVal;
        }

        if (op.type == TokenType::MINUS)
        {
            return leftVal - rightVal;
        }

        if (op.type == TokenType::STAR)
        {
            return leftVal * rightVal;
        }

        if (op.type == TokenType::SLASH)
        {
            return leftVal / rightVal;
        }

        if (op.type == TokenType::LESS)
        {
            return Value(leftVal < rightVal);
        }

        if (op.type == TokenType::NOT_EQUAL)
        {
            return Value(leftVal != rightVal);
        }

        if (op.type == TokenType::GREATER)
        {
            return Value(leftVal > rightVal);
        }

        if (op.type == TokenType::LESS_EQUAL)
        {
            return Value(leftVal <= rightVal);
        }

        if (op.type == TokenType::GREATER_EQUAL)
        {
            return Value(leftVal >= rightVal);
        }

        if (op.type == TokenType::OR) {
            return leftVal.AS_BOOLEAN ? leftVal : rightVal;
        }

        if (op.type == TokenType::AND) {
            return leftVal.AS_BOOLEAN ? leftVal : rightVal;
        }

        // TODO: Support plus, minus, multiply, divide, is, has, etc
        throw std::runtime_error("Unknown binary operator: " + op.lexeme);
    }

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override
    {

        std::string str = "BinaryExpr";

        str += "(operator: " + op.lexeme;

        if (left)
            str += ", left: " + left->toString();
        if (right)
            str += ", right: " + right->toString();

        str += ")";

        return str;
    }
};
