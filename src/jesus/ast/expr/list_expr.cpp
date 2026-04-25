#include "list_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "types/known_types.hpp"

Value ListExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitListExpr(*this);
}

std::shared_ptr<CreationType> ListExpr::getReturnType(ParserContext &ctx) const
{
    return KnownTypes::LIST;
};
