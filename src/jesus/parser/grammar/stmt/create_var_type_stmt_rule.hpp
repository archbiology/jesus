#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

class CreateVarTypeStmtRule
{

public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
