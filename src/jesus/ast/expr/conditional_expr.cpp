#include "conditional_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value ConditionalExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitConditional(*this);
}
