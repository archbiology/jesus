#include "binary_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value BinaryExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitBinary(*this);
}
