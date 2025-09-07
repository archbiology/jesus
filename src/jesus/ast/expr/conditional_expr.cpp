#include "conditional_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/creation_type.hpp"

Value ConditionalExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitConditional(*this);
}

std::shared_ptr<CreationType> ConditionalExpr::getReturnType(ParserContext &ctx) const
{
    return thenBranch->getReturnType(ctx);
};
