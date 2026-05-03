#pragma once

#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "../expr/ask_expr_rule.hpp"

class AssignStmtRule
{
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit AssignStmtRule(std::shared_ptr<IGrammarRule> expression)
        : expression(std::move(expression)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
