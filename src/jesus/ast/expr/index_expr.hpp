#pragma once

#include "assignable_expr.hpp"

REGISTER_FOR_UML(
    IndexExpr,
    .packageName("ast.expr")
        .parentsList({"AssignableExpr"})
        .fieldsList({"collection", "index"}));

/**
 * @brief Access list and dict indexes. E.g.: list[7]; config['db']
 */
class IndexExpr : public AssignableExpr
{
public:
    std::unique_ptr<Expr> collection;
    std::unique_ptr<Expr> index;

    IndexExpr(std::unique_ptr<Expr> collection, std::unique_ptr<Expr> index)
        : AssignableExpr(ExprKind::Other),
          collection(std::move(collection)), index(std::move(index)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return collection->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override;

    void assign(Interpreter &interpreter, const Value &value) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    std::string toString() const override
    {
        return collection->toString() + "[" + index->toString() + "]";
    }
};
