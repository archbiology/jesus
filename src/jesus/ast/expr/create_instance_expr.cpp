#include "../../interpreter/expr_visitor.hpp"

Value CreateInstanceExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitCreateInstanceExpr(*this);
}
