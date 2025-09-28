#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

/**
 * @brief Parser for 'repeat while (condition): amen
 */
class RepeatWhileStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
