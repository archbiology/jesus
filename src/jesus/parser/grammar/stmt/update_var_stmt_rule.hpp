#pragma once

#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "../expr/ask_expr_rule.hpp"

class UpdateVarStmtRule
{
    std::shared_ptr<IGrammarRule> expression;
    std::shared_ptr<AskExprRule> ask;

public:
    explicit UpdateVarStmtRule(std::shared_ptr<IGrammarRule> expression, std::shared_ptr<AskExprRule> ask_expr)
        : expression(std::move(expression)), ask(std::move(ask_expr)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx);
};
