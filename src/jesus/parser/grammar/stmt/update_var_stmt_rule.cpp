#include "update_var_stmt_rule.hpp"
#include "../../../ast/stmt/update_var_stmt.hpp"
#include "../../../ast/stmt/update_var_with_ask_stmt.hpp"
#include "../../../ast/expr/polimorfism_expr.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> UpdateVarStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::IDENTIFIER))
        return nullptr;

    const std::string varName = ctx.previous().lexeme;

    if (!ctx.match(TokenType::EQUAL))
        return nullptr;

    auto varType = ctx.getVarType(varName);
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

        return std::make_unique<UpdateVarWithAskStmt>(varType, varName, std::move(ask_expr));
    }

    std::unique_ptr<Expr> value = expression->parse(ctx);
    if (!value)
        throw std::runtime_error("Expected expression after '=' in update statement.");

    auto valueType = value->getReturnType(ctx);
    bool typesMatch = valueType->isA(varType);
    if (!typesMatch)
    {
        throw std::runtime_error("Variable '" + varName + "' expects a " + varType->name + ", but got: '" + valueType->name + "'.");
    }

    // If assigning a child instance to a parent variable, wrap in PolymorphismExpr
    if (valueType != varType)
    {
        value = std::make_unique<PolymorphismExpr>(std::move(value));
    }

    return std::make_unique<UpdateVarStmt>(varName, std::move(value));
}
