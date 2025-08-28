#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "create_var_stmt_rule.hpp"
#include "create_method_stmt_rule.hpp"

class CreateClassStmtRule
{
    std::shared_ptr<CreateVarStmtRule> createVar;
    std::shared_ptr<CreateMethodStmtRule> createMethod;

public:
    explicit CreateClassStmtRule(
        std::shared_ptr<CreateVarStmtRule> createVar,
        std::shared_ptr<CreateMethodStmtRule> createMethod)
        : createVar(std::move(createVar)), createMethod(std::move(createMethod)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
