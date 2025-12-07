#include "resist_stmt_rule.hpp"
#include "../../../ast/stmt/resist_stmt.hpp"
#include "../../../types/known_types.hpp"
#include "../jesus_grammar.hpp"

std::unique_ptr<Stmt> ResistStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::RESIST))
        return nullptr;

    auto ItsWritten = KnownTypes::EXCEPTION;
    auto exceptionType = ItsWritten;
    std::string exceptionTypeStr = exceptionType->name;

    if (ctx.match(TokenType::IDENTIFIER))
    {
        exceptionTypeStr = ctx.previous().lexeme;
        exceptionType = KnownTypes::resolve(exceptionTypeStr, "core"); // FIXME: not always 'core'. User define their own exceptions.
    }

    auto messageExpr = grammar::Expression->parse(ctx);
    if (!messageExpr)
        throw std::runtime_error("Expected message expression after exception type in 'resist'");

    return std::make_unique<ResistStmt>(exceptionType, std::move(messageExpr));
}
