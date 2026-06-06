#pragma once

#include "ast/stmt/stmt.hpp"
#include "parser/parser_context.hpp"

/**
 * @brief Parser for 'foreach' statements
 *
 * foreach key in items_list: amen
 * foreach key, value in dict_items.pairs(): amen
 */

class ForEachStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);

private:
    void consumeColon(ParserContext &ctx);

    std::vector<std::unique_ptr<Stmt>> parseBody(ParserContext &ctx);
};
