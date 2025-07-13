#include "literal_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value LiteralExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitLiteral(*this);
}
