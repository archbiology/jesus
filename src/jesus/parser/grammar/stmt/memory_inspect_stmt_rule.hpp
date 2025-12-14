#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

class MemoryInspectStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
