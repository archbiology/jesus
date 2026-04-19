#include "serve_stmt_rule.hpp"
#include "ast/stmt/serve_stmt.hpp"
#include "parser/parser_context.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include "types/known_types.hpp"

std::unique_ptr<Stmt> ServeStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::SERVE))
        return nullptr;

    std::unique_ptr<Expr> portExpr = nullptr;
    if (ctx.match(TokenType::ON))
    {
        portExpr = grammar::Expression->parse(ctx);
        if (!portExpr)
            throw std::runtime_error("Expected port number after 'serve on'");

        // -------------
        // Type checking
        // -------------
        auto varType = portExpr->getReturnType(ctx);
        if (!varType->isA(KnownTypes::INT))
        {
            throw std::runtime_error("Invalid port after 'serve on': expected number, got " + varType->name);
        }
    }

    return std::make_unique<ServeStmt>(std::move(portExpr));
}
