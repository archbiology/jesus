#pragma once

#include "../grammar_rule.hpp"
#include "ast/stmt/stmt.hpp"
#include "ast/stmt/print_stmt.hpp"
#include "ast/expr/literal_expr.hpp"
#include "types/known_types.hpp"

class PrintStmtRule
{
public:
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit PrintStmtRule(std::shared_ptr<IGrammarRule> expr)
        : expression(std::move(expr)) {}

    std::unique_ptr<Stmt> parse(ParserContext &ctx)
    {
        if (!ctx.matchAny({TokenType::SAY, TokenType::WARN, TokenType::SAYING, TokenType::WARNING}))
            return nullptr;

        const Token token = ctx.previous();
        const TokenType type = token.type;

        auto expr = expression->parse(ctx);

        StmtType stmtType = StmtType::SAY;
        switch (type)
        {
        case TokenType::WARN:
            stmtType = StmtType::WARN;
            break;

        case TokenType::SAYING:
            stmtType = StmtType::SAYING;
            break;

        case TokenType::WARNING:
            stmtType = StmtType::WARNING;
            break;
        }

        if (!expr)
        {
            if (type == TokenType::SAYING || type == TokenType::WARNING)
            {
                const auto keyword = token.lexeme;
                throw std::runtime_error(
                    "Expected a value after '" + keyword + "'. Example: " + keyword + " \"Jesus loves you\"");
            }

            expr = std::make_unique<LiteralExpr>(Value(""), KnownTypes::STRING);
        }

        return std::make_unique<PrintStmt>(stmtType, std::move(expr));
    }
};
