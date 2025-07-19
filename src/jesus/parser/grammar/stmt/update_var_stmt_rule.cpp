#include "update_var_stmt_rule.hpp"
#include "../../../ast/stmt/update_var_stmt.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> UpdateVarStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::IDENTIFIER))
        return nullptr;

    const std::string varName = ctx.previous().lexeme;

    if (!ctx.match(TokenType::EQUAL))
        return nullptr;

    std::unique_ptr<Expr> value = expression->parse(ctx);

    if (!value)
        throw std::runtime_error("Expected expression after '=' in update statement.");

    return std::make_unique<UpdateVarStmt>(varName, std::move(value));
}
