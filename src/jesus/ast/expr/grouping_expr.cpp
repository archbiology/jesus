#include "grouping_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value GroupingExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitGrouping(*this);
}

std::shared_ptr<CreationType> GroupingExpr::getReturnType(ParserContext &ctx) const
{
    return expression->getReturnType(ctx);
};
