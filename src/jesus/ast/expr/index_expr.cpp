#include "index_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "types/known_types.hpp"

Value IndexExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitIndexExpr(*this);
}

std::shared_ptr<CreationType> IndexExpr::getReturnType(ParserContext &ctx) const
{
    return KnownTypes::CREATION;
};
