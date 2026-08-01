#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include "../ast_node.hpp"
#include <memory>
#include <vector>


REGISTER_FOR_UML(
    RepeatTimesStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"countExpr", "body"}));

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

    void accept(StmtVisitor &visitor) const override;

    std::string toString() const override
    {
        std::string str = "RepeatTimesStmt(times: " + countExpr->toString() + ", body: [";
        bool isFirst = true;

        for (auto &stmt : body)
        {
            if (!isFirst)
                str += ",";

            str += "\n  " + stmt->toString();

            isFirst = false;
        }

        str += "])";

        return str;
    }
};
