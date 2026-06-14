#include "inspect_stmt_rule.hpp"
#include "ast/stmt/inspect_stmt.hpp"

std::unique_ptr<Stmt> InspectStmtRule::parse(ParserContext &ctx)
{
    InspectTarget target;

    if (ctx.match(TokenType::AST))
    {
        target = InspectTarget::AST;
    }
    else if (ctx.match(TokenType::BYTECODE))
    {
        target = InspectTarget::BYTECODE;
    }
    else if (ctx.match(TokenType::VM))
    {
        target = InspectTarget::VM;
    }
    else
    {
        return nullptr;
    }

    std::string name = "";

    if (ctx.match(TokenType::IDENTIFIER))
    {
        name = ctx.previous().lexeme;
    }

    return std::make_unique<InspectStmt>(target, name);
}
