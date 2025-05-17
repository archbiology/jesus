#pragma once

#include "ast_node.hpp"
#include "../spirit/heart.hpp"
#include <iostream>
#include <variant>


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
    Value evaluate(Heart *heart) override
    {
        Value result = condition->evaluate(heart);

        std::cout<< "🔥️ Resultado condicao: " << std::visit(make_string_functor(), result) + "\n";

        if (std::holds_alternative<double>(result) && std::get<bool>(result))
        {
            return ifTrue->evaluate(heart);
        }

        if (!ifFalse)
            return std::monostate{};

        return ifFalse->evaluate(heart);
    }

    void execute(Heart *heart) override
    {
        Value value = evaluate(heart);

        if (!std::holds_alternative<std::monostate>(value))
        {
            std::cout << std::visit(make_string_functor(), value) << std::endl;
            // std::cout << std::get<decltype(value)>(value) << std::endl;
            // std::visit([](auto &&val) { std::cout << val << std::endl; }, value);
        }
    }

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override  {
        std::string str = "ConditionalExpressionNode";

        if (condition) str += "(condition: " + condition->toString();
        if (ifTrue) str += ", ifTrue: " + ifTrue->toString();
        if (ifFalse) str += ", ifFalse: " + ifFalse->toString();

        str += ")";

        return str;
    }
};