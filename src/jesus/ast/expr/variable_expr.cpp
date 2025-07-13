#include "variable_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value VariableExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitVariable(*this);
}
