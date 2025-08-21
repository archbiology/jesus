#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "create_var_stmt_rule.hpp"

class CreateClassStmtRule
{
    std::shared_ptr<CreateVarStmtRule> createVar;

public:
    explicit CreateClassStmtRule(std::shared_ptr<CreateVarStmtRule> createVar)
        : createVar(std::move(createVar)){}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
