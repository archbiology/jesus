#include "parity_check_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/known_types.hpp"

Value ParityCheckExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitParityCheckExpr(*this);
}

std::shared_ptr<CreationType> ParityCheckExpr::getReturnType(ParserContext &ctx) const
{
    return KnownTypes::TRUTH;
}
