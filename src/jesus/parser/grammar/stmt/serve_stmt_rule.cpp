#include "serve_stmt_rule.hpp"
#include "../../../ast/stmt/serve_stmt.hpp"
#include "../../parser_context.hpp"

std::unique_ptr<Stmt> ServeStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::SERVE))
        return nullptr;

    return std::make_unique<ServeStmt>();
}
