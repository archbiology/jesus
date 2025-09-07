#include "method_call_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../interpreter/runtime/method.hpp"

Value MethodCallExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitMethodCallExpr(*this);
}

std::shared_ptr<CreationType> MethodCallExpr::getReturnType(ParserContext &ctx) const
{
    return method->getReturnType(ctx);
};
