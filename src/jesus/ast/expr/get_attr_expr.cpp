#include "get_attr_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value GetAttributeExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitGetAttribute(*this);
}
