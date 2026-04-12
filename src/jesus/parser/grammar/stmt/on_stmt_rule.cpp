#include "on_stmt_rule.hpp"

#include "ast/stmt/on_stmt.hpp"
#include "ast/stmt/return_stmt.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include "parser/parser_context.hpp"

std::unique_ptr<Stmt> OnStmtRule::parse(ParserContext &ctx)
{
    // expect: on
    if (!ctx.match(TokenType::ON))
        return nullptr;

    // protocol: http
    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected protocol after 'on'");
    std::string protocol = ctx.previous().lexeme;

    // path: '/status'
    if (!ctx.match(TokenType::RAW_STRING))
        throw std::runtime_error("Expected route path");
    std::string path = ctx.previous().literal.toString();

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after route definition");

    // body
    std::vector<std::shared_ptr<Stmt>> body;

    ctx.consumeAllNewLines();
    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (ctx.match(TokenType::RETURN))
        {
            std::unique_ptr<Expr> returnExpr = nullptr;

            if (!ctx.check(TokenType::NEWLINE) && !ctx.check(TokenType::AMEN))
            {
                // Parse the expression after 'return'
                returnExpr = grammar::Expression->parse(ctx);
            }

            body.push_back(std::make_shared<ReturnStmt>(std::move(returnExpr)));
        }
        else
        {
            throw std::runtime_error("Unexpected statement inside route body.");
        }
        ctx.consumeAllNewLines();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close route body.");

    return std::make_unique<OnStmt>(
        protocol,
        path,
        std::move(body));
}
