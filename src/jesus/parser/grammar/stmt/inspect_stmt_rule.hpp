#pragma once

#include "parser/parser_context.hpp"
#include "ast/stmt/stmt.hpp"

class InspectStmtRule
{
public:

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
