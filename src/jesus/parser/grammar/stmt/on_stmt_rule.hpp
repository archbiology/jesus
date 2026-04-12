#pragma once
#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"

/**
 * @brief Parser for 'on':

    on http '/status':
        return 1
    amen
 */
class OnStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
