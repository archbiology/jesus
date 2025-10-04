#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

/**
 * @brief Parser for 'if-otherwise' statements
 *
 *  if name == "Jesus":
 *      say "Jesus, name above all names."
 *  amen
 */
class IfStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);

    std::vector<std::unique_ptr<Stmt>> parseBody(ParserContext &ctx);
};
