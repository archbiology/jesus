#pragma once

#include "expr.hpp"

/**
 * @brief Represents an 'ask' expression in the Jesus language.
 *
 * This expression holds a prompt, which can be either a string literal
 * or a variable expression that will be evaluated to produce the question
 * asked to the user.
 *
 * "Ask, and it will be given to you; seek, and you will find; knock, and it will be opened to you." — Matthew 7:7
 */
class AskExpr : public Expr
{
public:
    /**
     * @brief The prompt of the question, which can be an expression producing text (literal or variable)
     */
    std::unique_ptr<Expr> prompt;

public:
    /**
     * @brief Constructs the AskExpr with a given prompt expression.
     *
     * @param prompt An expression representing the question to be asked.
     */
    explicit AskExpr(std::unique_ptr<Expr> prompt) : prompt(std::move(prompt)) {}

    /**
     * @brief Evaluates the 'prompt' expression.
     *
     * @return The string representing the question.
     */
    Value evaluate(Heart *heart) const override;

    /**
     * @brief Gets the answer from the user.
     *
     * @return The value received as the user's answer.
     */
    Value accept(ExprVisitor &visitor) const override;

    std::string toString() const override
    {
        // TODO: Add toJesus() that returns: return "ask \"" + prompt + "\"";
        //  'jesus refine' 7 times: Psalm 12:6
        return "AskExpr(" + prompt->toString() + ")";
    }
};
