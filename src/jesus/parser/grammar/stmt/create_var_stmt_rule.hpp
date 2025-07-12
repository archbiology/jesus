#pragma once

#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"

class CreateVarStmtRule
{
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit CreateVarStmtRule(std::shared_ptr<IGrammarRule> expr)
        : expression(std::move(expr)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
