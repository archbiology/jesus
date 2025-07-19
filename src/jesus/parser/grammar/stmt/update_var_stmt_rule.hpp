#pragma once

#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"

class UpdateVarStmtRule
{
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit UpdateVarStmtRule(std::shared_ptr<IGrammarRule> expression)
        : expression(std::move(expression)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
