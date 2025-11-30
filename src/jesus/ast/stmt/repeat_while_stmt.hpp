#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <memory>
#include <vector>

REGISTER_FOR_UML(
    RepeatWhileStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"condition", "body"}));

/**
 * @brief
 *
 * repeat while condition:
 *   say "Still going..."
 */
class RepeatWhileStmt : public Stmt
{
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;

    RepeatWhileStmt(
        std::unique_ptr<Expr> condition,
        std::vector<std::unique_ptr<Stmt>> body) : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor &visitor) const override;
};
