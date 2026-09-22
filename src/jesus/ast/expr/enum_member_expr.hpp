#pragma once

#include "expr.hpp"
#include "../../types/creation_type.hpp"
#include <string>
#include <memory>

REGISTER_FOR_UML(
    EnumMemberExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"enumType", "memberName", "label", "value"}));

class EnumMemberExpr : public Expr
{
  public:
    std::shared_ptr<CreationType> enumType;
    const std::string memberName;
    const std::string label;
    const int value;

    EnumMemberExpr(
        std::shared_ptr<CreationType> enumType, const std::string &memberName, const std::string &label, int value)
        : enumType(std::move(enumType)), memberName(memberName), label(label), value(value), Expr(ExprKind::Literal)
    {
    }

    Value evaluate(std::shared_ptr<Heart> heart) const override;

    Value accept(ExprVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    bool canBeUsedInFormattedString() const override { return true; }

    std::string toString() const override { return memberName; }
};
