#include "unary_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value UnaryExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitUnary(*this);
}
