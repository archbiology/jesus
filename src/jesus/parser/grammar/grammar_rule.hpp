#pragma once
#include "../parser_context.hpp"

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
    virtual bool parse(ParserContext &ctx) = 0;

    virtual std::string toString()
    {
        return "IGrammarRule";
    }
};
