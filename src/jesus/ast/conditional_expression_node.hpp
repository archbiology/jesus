#pragma once

#include "ast_node.hpp"
#include "spirit/heart.hpp"
#include <iostream>

/**
 * @brief Represents `reveal "adult" if age >= 18 otherwise "young"`
 *
 * 📖 "I have set before you life and death, blessings and curses. Now choose life, so that you and your children may live."
 * — Deuteronomy 30:19
 *
 * This node evaluates a condition and returns the result of either the
 * `ifTrue` or `ifFalse` branch based on the logic outcome of the condition.
 *
 * It enables decision-making logic in the language's syntax, allowing
 * expressions such as: condition ? ifTrue : ifFalse
 */
struct ConditionalExpressionNode : public ASTNode
{
    ASTNode *condition;
    ASTNode *ifTrue;
    ASTNode *ifFalse;

    /**
     * @brief Construct a new ConditionalExpressionNode object
     *
     * Represents a conditional expression (similar to a ternary operation).
     *
     * 📖 "I have set before you life and death, blessings and curses. Now choose life, so that you and your children may live."
     * — Deuteronomy 30:19
     *
     * @param condition The condition to evaluate (should return "true" or "false").
     * @param ifTrue The expression to evaluate if the condition is true.
     * @param ifFalse The expression to evaluate if the condition is false.
     */
    ConditionalExpressionNode(ASTNode *condition, ASTNode *ifTrue, ASTNode *ifFalse)
        : condition(condition), ifTrue(ifTrue), ifFalse(ifFalse) {}

    /**
     * @brief  Evaluates a condition and returns the result of either the
     * `ifTrue` or `ifFalse` branch based on the logic outcome of the condition.
     *
     * - If the condition evaluates to "true", it evaluates and returns the `ifTrue` branch.
     * - If the condition evaluates to anything else (or fails), it evaluates and returns the `ifFalse` branch if available.
     *
     * @param heart The "Symbol table"
     * @return std::optional<std::string>
     */
    std::optional<std::string> evaluate(Heart *heart) override
    {
        auto result = condition->evaluate(heart);

        if (result.has_value() && result.value() == "true")
        {
            return ifTrue->evaluate(heart);
        }

        if (!ifFalse)
            return std::nullopt;

        return ifFalse->evaluate(heart);
    }

    void execute(Heart *heart)
    {
        auto value = evaluate(heart);
        if (value.has_value())
        {
            std::cout << value.value() << std::endl;
        }
    }
};