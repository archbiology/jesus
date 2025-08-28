#pragma once

#include <memory>
#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "create_var_stmt_rule.hpp"
#include "update_var_stmt_rule.hpp"

class CreateMethodStmtRule
{
    std::shared_ptr<CreateVarStmtRule> createVar;
    std::shared_ptr<UpdateVarStmtRule> updateVar;

public:
    explicit CreateMethodStmtRule(std::shared_ptr<CreateVarStmtRule> createVar, std::shared_ptr<UpdateVarStmtRule> updateVar)
        : createVar(std::move(createVar)), updateVar(std::move(updateVar)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
