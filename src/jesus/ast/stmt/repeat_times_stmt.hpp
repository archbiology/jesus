#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include "../ast_node.hpp"
#include <memory>
#include <vector>

/**
 * repeat 3 times:
 *   say "Holy!"
 */
class RepeatTimesStmt : public Stmt
{
public:
    int count = 0;
    std::unique_ptr<Expr> countExpr;
    std::vector<std::unique_ptr<Stmt>> body;
    std::unique_ptr<ASTNode> body2;

    RepeatTimesStmt(std::unique_ptr<Expr> countExpr,
                    std::vector<std::unique_ptr<Stmt>> body)
        : countExpr(std::move(countExpr)), body(std::move(body)) {}

    RepeatTimesStmt(int count, std::unique_ptr<Stmt> body)
        : count(count), body2(std::move(body)) {}

    void accept(StmtVisitor &visitor) const override;
};
