#pragma once

#include "combinators/forward_rule.hpp"
#include "grammar_aliases.hpp" // for Number, String, etc.
#include "operators.hpp"

#include "expr/atomic/operators/addition_rule.hpp"
#include "expr/atomic/operators/comparison_rule.hpp"
#include "expr/atomic/operators/equality_rule.hpp"
#include "expr/atomic/operators/logical_and_rule.hpp"
#include "expr/atomic/operators/logical_or_rule.hpp"
#include "expr/atomic/operators/multiplication_rule.hpp"
#include "expr/atomic/operators/versus_rule.hpp"

#include "expr/atomic/literals/number_rule.hpp"
#include "expr/atomic/literals/string_rule.hpp"
#include "expr/atomic/literals/variable_rule.hpp"
#include "expr/atomic/literals/yes_no_rule.hpp"
#include "expr/atomic/literals/born_rule.hpp"
#include "expr/atomic/literals/weekday_rule.hpp"

#include "expr/ask_expr_rule.hpp"
#include "expr/conditional_expr_rule.hpp"
#include "stmt/create_var_type_stmt_rule.hpp"
#include "stmt/create_var_stmt_rule.hpp"
#include "stmt/update_var_stmt_rule.hpp"
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
    inline auto LogicalAnd = std::make_shared<LogicalAndRule>(Equality);
    inline auto Versus = std::make_shared<VersusRule>(LogicalAnd, LogicalAnd);
    inline auto LogicalOr = std::make_shared<LogicalOrRule>(Versus);
    inline auto Conditional = std::make_shared<ConditionalExprRule>(LogicalOr);
    inline auto Expression = Conditional;

    inline auto Number = std::make_shared<NumberRule>();
    inline auto String = std::make_shared<StringRule>();
    inline auto YesNo = std::make_shared<YesNoRule>(); // yes|no
    inline auto Sex = std::make_shared<BornRule>(); // male|female
    inline auto Weekday = std::make_shared<WeekdayRule>(); // lighday|skyday|treeday|lampday|fishday|walkday|shabbat
    inline auto Variable = std::make_shared<VariableRule>();
    inline auto Ask = std::make_shared<AskExprRule>();

    /**
     * @brief Primary is anything that can be evaluated directly: number, string, or a grouped expression.
     */
    inline auto Primary = Number | String | YesNo | Sex | Weekday | Variable | Group(Expression);

    // ----------
    // Statements
    // ----------
    inline auto CreateVarType = std::make_shared<CreateVarTypeStmtRule>();
    inline auto CreateVar = std::make_shared<CreateVarStmtRule>(Expression, Ask);
    inline auto UpdateVar = std::make_shared<UpdateVarStmtRule>(Expression);

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
