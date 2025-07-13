#pragma once

#include "../../../grammar_rule.hpp"

/**
 * @brief Parses weekdays literals: `lighday`, `skyday`, `treeday`, `lampday`, `fishday`, `walkday`, `shabbat`.
 *
 * "And God blessed the seventh day and made it holy, because on it he rested from all the work of creating that he had done."
 * — Genesis 2:3
 */
class WeekdayRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &) const override
    {
        return "Weekday";
    }
};
