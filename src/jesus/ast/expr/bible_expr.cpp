#include "bible_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value BibleExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitBibleExpr(*this);
}

std::shared_ptr<CreationType> BibleExpr::getReturnType(ParserContext &ctx) const
{
    return reference->getReturnType(ctx);
};
