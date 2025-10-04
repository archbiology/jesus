#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"

/**
 * if name == 'Jesus Christ':
 *    say 'Lamb of God'
 * amen
 */
class IfStmt : public Stmt
{
public:
  const std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> thenBranch;
  std::vector<std::unique_ptr<Stmt>> otherwiseBranch;

  IfStmt(std::unique_ptr<Expr> condition,
         std::vector<std::unique_ptr<Stmt>> thenBranch,
         std::vector<std::unique_ptr<Stmt>> otherwiseBranch)
      : condition(std::move(condition)),
        thenBranch(std::move(thenBranch)),
        otherwiseBranch(std::move(otherwiseBranch)) {}

  void accept(StmtVisitor &visitor) const override;
};
