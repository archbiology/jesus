#pragma once
#include "../grammar_rule.hpp"
#include <memory>

/**
 * @brief A forward-declared grammar rule placeholder that can be resolved later.
 *
 * It allows the definition of recursive grammar structures by acting as a proxy
 * to a real rule that may not have been defined yet at the time of declaration.
 *
 * For example, in EBNF we often write:
 *     expression = term | expression "+" term
 * Such definitions are inherently recursive, and this class enables us to express that in C++.
 *
 * Usage:
 *     auto Expression = std::make_shared<ForwardRule>();
 *     Expression->setTarget(...); // assign the real rule after defining it
 *
 * ✝️ “The heart of man plans his way, but the Lord establishes his steps.”
 *     — Proverbs 16:9
 *
 * Just as this class temporarily plans a path forward, the real structure is
 * revealed and resolved in its time.
 */
class ForwardRule : public IGrammarRule
{
private:
    std::string name;
    std::shared_ptr<IGrammarRule> target;

public:
    explicit ForwardRule(std::string name = "Forward") : name(std::move(name)) {}

    /**
     * @brief Sets the actual rule this forward reference will resolve to.
     *
     * This should be called **once** after all grammar rules are defined.
     * The ForwardRule then delegates parsing and string conversion to the
     * assigned target rule.
     *
     * ✝️ "Write the vision; make it plain on tablets, so he may run who reads it."
     *     — Habakkuk 2:2
     * Like a vision that becomes clear in time, this rule points ahead to what will be revealed.
     *
     * @param rule A shared pointer to the actual grammar rule this forward should resolve to.
     */
    void setTarget(std::shared_ptr<IGrammarRule> rule)
    {
        target = std::move(rule);
    }

    std::unique_ptr<Expr> parse(ParserContext &ctx) override
    {
        if (!target)
            throw std::runtime_error("ForwardRule target not set");

        return target->parse(ctx);
    }

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        if (visitedTable.count(this))
            return name + "(...)";

        visitedTable.insert(this);

        return target ? target->toStr(visitedTable) : "UnresolvedForwardRule";
    }
};
