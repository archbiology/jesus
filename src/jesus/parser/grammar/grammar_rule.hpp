#pragma once
#include "../parser_context.hpp"
#include "../../ast/expr/expr.hpp"
#include <unordered_set>

class IGrammarRule; // Forward declaration
using GrammarRuleHashTable = std::unordered_set<const IGrammarRule *>;

/**
 * @brief Base interface for all grammar rules in the Jesus Language parser.
 *
 * "Do your best to present yourself to God as one approved,
 * a worker who does not need to be ashamed and who correctly handles the word of truth."
 * — 2 Timothy 2:15
 */
class IGrammarRule
{
public:
    virtual ~IGrammarRule() = default;

    /**
     * Tries to parse the rule from the given context.
     *
     * @param ctx The parser context containing tokens and state.
     * @return true if the rule matched and consumed tokens.
     */
    virtual std::unique_ptr<Expr> parse(ParserContext &ctx) = 0;

    /**
     * @brief Return a string version of the rule
     *
     * ⚠️ Please do not override this. Override `toStr` instead.
     *
     * @return std::string
     */
    virtual std::string toString() const final // final = "can't" be overwritten
    {
        GrammarRuleHashTable visitedTable;
        return toStr(visitedTable);
    }

    /**
     * @brief Generates a string representation of the grammar rule,
     *        avoiding infinite recursion by tracking visited rules.
     *
     * This method is used internally by `toString()` to safely convert
     * the grammar rule structure into a human-readable string,
     * even when rules are recursively defined (e.g., Expression).
     *
     * The `visitedTable` is a set of rule pointers that have already
     * been visited in the current traversal, preventing recursion like:
     *     Expression → Group(Expression) → Group(Group(Expression)) → ...
     *
     * ⚠️ Override this method in derived grammar rule classes to define
     *    how each specific rule should be printed, avoiding infinite recursion.
     *
     * The purposes of a person’s heart are deep waters, but one who has insight draws them out.  — Psalm 25:9
     *
     * @param visitedTable A set tracking visited rules during traversal.
     * @return A human-readable string of the rule structure.
     */
    virtual std::string toStr(GrammarRuleHashTable &visitedTable) const = 0;
};
