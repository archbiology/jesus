#pragma once
#include "stmt.hpp"
#include "ast/expr/assignable_expr.hpp"

REGISTER_FOR_UML(
    AssignStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"target", "value"}));

class AssignStmt : public Stmt
{
public:
  std::unique_ptr<AssignableExpr> target;
  std::unique_ptr<Expr> value;

  AssignStmt(std::unique_ptr<AssignableExpr> target, std::unique_ptr<Expr> value)
      : target(std::move(target)), value(std::move(value)) {}

  std::string toString() const override
  {
    return "AssignStmt(target: " + target->toString() + ", value: " + value->toString() + ")";
  }

  void accept(StmtVisitor &visitor) const override;
};
