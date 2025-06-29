#pragma once

#include "combinators/forward_rule.hpp"
#include "grammar_aliases.hpp" // for Number, String, etc.
#include "operators.hpp"
#include "primitives/addition_rule.hpp"
#include "primitives/comparison_rule.hpp"
#include "primitives/equality_rule.hpp"
#include "primitives/multiplication_rule.hpp"
#include "primitives/versus_rule.hpp"
#include "unary_rule.hpp"

/**
 * @brief Central EBNF-style grammar definition for the Jesus language.
 *
 * “Do your best to present yourself to God as one approved, a worker who does not need to be ashamed and who correctly handles the word of truth.” - 2 Timothy 2:15
 *
 * 🌟 Central place for all EBNF rules (declarative rules)
 *
 * This file contains the full composition of the Jesus language's grammar
 * using modular EBNF-style rules. The goal is clarity, elegance, and maintainability.
 */
namespace grammar
{

    /**
     * @brief Create a forward declaration for ExpressionRule
     *
     * It solves the following problem: Primary refers to ExpressionRule, and ExpressionRule refers to Primary.
     * That's a cyclic dependency — a common issue in recursive descent parsers.
     *
     * It acts as a placeholder that we can define later
     */

    inline auto Unary = std::make_shared<ForwardRule>("Unary");
    inline auto Multiplication = std::make_shared<MultiplicationRule>(Unary);
    inline auto Addition = std::make_shared<AdditionRule>(Multiplication);
    inline auto Comparison = std::make_shared<ComparisonRule>(Addition);
    inline auto Equality = std::make_shared<EqualityRule>(Comparison);
    inline auto Expression = Equality;
    inline auto Versus = std::make_shared<VersusRule>(Addition, Addition);

    /**
     * @brief Primary is anything that can be evaluated directly: number, string, or a grouped expression.
     */
    inline auto Primary = Number | String | Group(Expression);

    /**
     * @brief Set the Expression rule to something (for now just Primary)
     *
     *  Expression, in its full form, will later be:
     *  Expression → Or → And → Equality → Comparison → Term → Factor → Primary
     *
     *  But for now, Expression := Primary
     *
     */
    inline void initializeGrammar()
    {
        Unary->setTarget(std::make_shared<UnaryRule>(Primary));
    }
}
