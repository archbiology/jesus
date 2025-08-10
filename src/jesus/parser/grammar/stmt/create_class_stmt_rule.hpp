#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

class CreateClassStmtRule
{

public:
    explicit CreateClassStmtRule() {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
