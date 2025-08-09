#include "update_var_stmt_rule.hpp"
#include "../../../ast/stmt/update_var_stmt.hpp"
#include "../../../ast/stmt/update_var_with_ask_stmt.hpp"
#include "../../../interpreter/interpreter.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> UpdateVarStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::IDENTIFIER))
        return nullptr;

    const std::string varName = ctx.previous().lexeme;

    if (!ctx.match(TokenType::EQUAL))
        return nullptr;

    // return KnownTypes::resolve(varName);
    const CreationType *varType = ctx.getVarType(varName);
    if (!varType)
    {
        throw std::runtime_error("Variable '" + varName + "' not found. Are you sure it was declared?");
    }

    int snapshot = ctx.snapshot();
    if (ctx.match(TokenType::ASK))
    {
        ctx.restore(snapshot); // because 'ask->parse(ctx)' below expects TokenType::ASK, but since it matched, it already advanced.

        std::unique_ptr<Expr> ask_expr = ask->parse(ctx);
        if (!ask_expr)
            throw std::runtime_error("Expected a text literal or a text-typed variable after 'ask' (e.g., ask \"What is your name again?\" or ask question).");

        return std::make_unique<UpdateVarWithAskStmt>(*varType, varName, std::move(ask_expr));
    }

    std::unique_ptr<Expr> value = expression->parse(ctx);
    if (!value)
        throw std::runtime_error("Expected expression after '=' in update statement.");

    // Evaluating functions that could send an 'ok' to bomb explosions is not safe.
    // Let's warn this until we can check the return type from functions
    std::cerr << "🔴️ FIXME: Should not call interpreter here. Remove this once functions and static type inference are implemented.\n";
    Value evaluated = ctx.interpreter->evaluate(value);
    if (!varType->validate(evaluated))
    {
        throw std::runtime_error("Variable '" + varName+ "' expects a " + varType->name + ", but got: '" + evaluated.toString() + "'.");
    }

    return std::make_unique<UpdateVarStmt>(varName, std::move(value));
}
