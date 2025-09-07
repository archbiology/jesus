#include "binary_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/creation_type.hpp"

Value BinaryExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitBinary(*this);
}

std::shared_ptr<CreationType> BinaryExpr::getReturnType(ParserContext &ctx) const
{
    return right->getReturnType(ctx);
};
