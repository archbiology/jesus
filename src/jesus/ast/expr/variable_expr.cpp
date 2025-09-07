#include "variable_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../parser/parser_context.hpp"

Value VariableExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitVariable(*this);
}

std::shared_ptr<CreationType> VariableExpr::getReturnType(ParserContext &ctx) const
{
    return ctx.getVarType(name);
};
