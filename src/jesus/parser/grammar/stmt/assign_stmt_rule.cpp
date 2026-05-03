#include "assign_stmt_rule.hpp"
#include "ast/stmt/assign_stmt.hpp"
#include "ast/expr/assignable_expr.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> AssignStmtRule::parse(ParserContext &ctx)
{
    int start = ctx.snapshot();
    auto target = expression->parse(ctx);
    if (!target)
        return nullptr;

    if (!ctx.match(TokenType::EQUAL))
    {
        ctx.restore(start);
        return nullptr;
    }

    auto value = expression->parse(ctx);
    if (!value)
        throw std::runtime_error("Expected value after '='.");

    auto assignable = dynamic_cast<AssignableExpr *>(target.get());
    if (!assignable)
    {
        throw std::runtime_error("This value cannot be changed.");
    }

    std::unique_ptr<AssignableExpr> assignablePtr(static_cast<AssignableExpr *>(target.release()));
    return std::make_unique<AssignStmt>(std::move(assignablePtr), std::move(value));
}
