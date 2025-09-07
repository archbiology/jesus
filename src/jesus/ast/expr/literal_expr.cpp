#include "literal_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value LiteralExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitLiteral(*this);
}

std::shared_ptr<CreationType> LiteralExpr::getReturnType(ParserContext &ctx) const
{
    return type;
};
