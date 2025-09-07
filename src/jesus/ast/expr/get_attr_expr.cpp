#include "get_attr_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/creation_type.hpp"
#include "../../parser/parser_context.hpp"

Value GetAttributeExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitGetAttribute(*this);
}

std::shared_ptr<CreationType> GetAttributeExpr::getReturnType(ParserContext &ctx) const
{
    return ctx.getVarType(attribute);
};
