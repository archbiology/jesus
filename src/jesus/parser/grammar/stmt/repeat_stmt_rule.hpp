#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

/**
 * @brief Parser for 'repeat' statements
 *
 *  repeat forever: amen
 *  repeat <n> times: amen
 *  repeat while <condition>: amen
 */
class RepeatStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);

    void consumeColon(ParserContext &ctx);

    std::vector<std::unique_ptr<Stmt>> parseBody(ParserContext &ctx);
};
