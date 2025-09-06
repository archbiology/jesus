#include "method_call_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value MethodCallExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitMethodCallExpr(*this);
}
