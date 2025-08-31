#pragma once

#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"
#include "../../../ast/stmt/print_stmt.hpp"

class PrintStmtRule
{
public:
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit PrintStmtRule(std::shared_ptr<IGrammarRule> expr)
        : expression(std::move(expr)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx)
    {
        if (!ctx.matchAny({TokenType::SAY, TokenType::WARN}))
            return nullptr;

        TokenType type = ctx.previous().type; // SAY or WARN

        auto expr = expression->parse(ctx);
        if (!expr)
            throw std::runtime_error("Expected expression after 'say'/'warn'");

        return std::make_unique<PrintStmt>(
            type == TokenType::SAY ? StmtType::SAY : StmtType::WARN,
            std::move(expr));
    }
};
