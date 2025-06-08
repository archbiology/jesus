#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <memory>
#include <vector>

/**
 * repeat 3 times:
 *   say "Holy!"
 */
class RepeatTimesStmt : public Stmt
{
public:
    std::unique_ptr<Expr> countExpr;
    std::vector<std::unique_ptr<Stmt>> body;

    RepeatTimesStmt(std::unique_ptr<Expr> countExpr,
                    std::vector<std::unique_ptr<Stmt>> body)
        : countExpr(std::move(countExpr)), body(std::move(body)) {}
};
