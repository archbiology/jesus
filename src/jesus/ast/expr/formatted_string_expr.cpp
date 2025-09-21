#include "formatted_string_expr.hpp"
#include "../../types/known_types.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value FormattedStringExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitFormattedStringExpr(*this);
}

std::shared_ptr<CreationType> FormattedStringExpr::getReturnType(ParserContext &ctx) const
{
    return KnownTypes::STRING;
}
