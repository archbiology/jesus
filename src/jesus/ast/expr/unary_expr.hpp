#pragma once

#include "expr.hpp"
#include "../../lexer/token.hpp"
#include <memory>

/**
 * @brief Represents a unary operation like `not lie` or `-problems`.
 *
 * Unary expressions modify a single operand, often inverting or transforming it.
 * Just as God works within us — transforming darkness into light, sorrow into joy
 * — a unary operation reflects deep, foundational change.
 *
 * "I will give you a new heart and put a new spirit in you; I will remove from
 * you your heart of stone and give you a heart of flesh."
 * — Ezekiel 36:26
 */
class UnaryExpr : public Expr
{
public:
    Token op;
    std::unique_ptr<Expr> right;

    /**
     * @brief Constructs a UnaryExpr with an operator and its operand.
     *
     * Applies a transformation to a single expression — like negating a number
     * or reversing a condition. It symbolizes how a small shift in one element
     * can change the whole outcome.
     *
     * "Be transformed by the renewal of your mind, that by testing you may
     * discern what is the will of God, what is good and acceptable and perfect."
     * — Romans 12:2
     *
     * @param op    The unary operator token (e.g., '-', 'not').
     * @param right The operand expression the operator acts upon (e.g., sin).
     */
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        Value rightVal = right->evaluate(heart);

        if (op.type == TokenType::PLUS)
        {

            if (rightVal.IS_NUMBER)
            {
                return rightVal;
            }

            throw std::runtime_error("Unary '+' applied to non-number: " + rightVal.toString());
        }

        if (op.type == TokenType::MINUS)
        {

            if (rightVal.IS_NUMBER)
            {
                return Value(-rightVal.toNumber());
            }

            throw std::runtime_error("Unary '-' applied to non-number: " + rightVal.toString());
        }

        if (op.type == TokenType::NOT)
        {
            return Value(!rightVal.AS_BOOLEAN);
        }

        throw std::runtime_error("Unknown unary operator: " + op.lexeme);
    }

    Value accept(ExprVisitor &visitor) const override;

    bool canEvaluateAtParseTime() const override
    {
        return right->canEvaluateAtParseTime();
    }

    /**
     * @brief Get the return type of the expression, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const override { return "UnaryExpr(" + op.lexeme + ", " + right->toString() + ")"; }
};
