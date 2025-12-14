#include "ast_inspect_stmt_rule.hpp"
#include "../../../ast/stmt/ast_inspect_stmt.hpp"

std::unique_ptr<Stmt> AstInspectStmtRule::parse(ParserContext &ctx)
{

    if (!ctx.match(TokenType::AST)) // token for 'ast'
        return nullptr;

    std::string name = "";

    if (ctx.match(TokenType::IDENTIFIER))
    {
        name = ctx.previous().lexeme;
    }

    return std::make_unique<AstInspectStmt>(name);
}
