#include "enum_member_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../interpreter/runtime/enum_instance.hpp"

Value EnumMemberExpr::accept(ExprVisitor &visitor) const { return visitor.visitEnumMember(*this); }

Value EnumMemberExpr::evaluate(std::shared_ptr<Heart> heart) const
{
    return Value(std::make_shared<EnumInstance>(enumType, memberName, label, value));
}

std::shared_ptr<CreationType> EnumMemberExpr::getReturnType(ParserContext &ctx) const { return enumType; }
