#pragma once

#include <memory>
#include "expr/atomic/literals/number_rule.hpp"
#include "expr/atomic/literals/string_rule.hpp"
#include "group_rule.hpp"

/**
 * @file grammar_aliases.hpp
 * @brief Pure-EBNF aliases for internal GrammarRules.
 * Use this to define grammars in expressive EBNF style.
 *
 * “Do your best to present yourself to God as one approved, a worker who does not need to be ashamed and who correctly handles the word of truth.”
 * - 2 Timothy 2:15
 */

inline auto Number = std::make_shared<NumberRule>();

inline auto String = std::make_shared<StringRule>();

// Group is a function because it requires an inner expression rule
inline auto Group = [](auto inner)
{
    return std::make_shared<GroupRule>(inner);
};
