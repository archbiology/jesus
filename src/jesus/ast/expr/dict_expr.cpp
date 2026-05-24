#include "dict_expr.hpp"
#include "interpreter/expr_visitor.hpp"

Value DictExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitDictExpr(*this);
}
