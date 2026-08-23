#pragma once

#include <memory>
#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "create_var_stmt_rule.hpp"
#include "update_var_stmt_rule.hpp"
#include "print_stmt_rule.hpp"
#include "assign_stmt_rule.hpp"

class CreateMethodStmtRule
{
    std::shared_ptr<CreateVarStmtRule> createVar;
    std::shared_ptr<UpdateVarStmtRule> updateVar;
    std::shared_ptr<PrintStmtRule> printStmt;
    std::shared_ptr<AssignStmtRule> assign;

public:
    explicit CreateMethodStmtRule(
        std::shared_ptr<CreateVarStmtRule> createVar,
        std::shared_ptr<UpdateVarStmtRule> updateVar,
        std::shared_ptr<PrintStmtRule> printStmt,
        std::shared_ptr<AssignStmtRule> assign)
        : createVar(std::move(createVar)), updateVar(std::move(updateVar)),
          printStmt(std::move(printStmt)), assign(std::move(assign)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
