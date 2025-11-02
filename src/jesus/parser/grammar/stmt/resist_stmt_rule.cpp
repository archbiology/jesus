#include "resist_stmt_rule.hpp"
#include "../../../ast/stmt/resist_stmt.hpp"
#include "../jesus_grammar.hpp"

std::unique_ptr<Stmt> ResistStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::RESIST))
        return nullptr;

    std::string exceptionType = "ItsWritten";

    if (ctx.match(TokenType::IDENTIFIER))
    {
        exceptionType = ctx.previous().lexeme;
    }

    auto messageExpr = grammar::Expression->parse(ctx);
    if (!messageExpr)
        throw std::runtime_error("Expected message expression after exception type in 'resist'");

    return std::make_unique<ResistStmt>(exceptionType, std::move(messageExpr));
}
