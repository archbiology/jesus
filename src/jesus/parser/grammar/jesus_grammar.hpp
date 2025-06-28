#pragma once

#include "grammar_aliases.hpp" // for Number, String, etc.
#include "operators.hpp"

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

    inline auto Primary = Number | String;

    inline auto ExpressionRule = Primary | Group(Number | String); // simplistic for now

}
