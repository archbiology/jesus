#pragma once
#include "../grammar_rule.hpp"
#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

/**
 * @brief Parser for 'serve', that runs an Http Server.
 */
class ServeStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
