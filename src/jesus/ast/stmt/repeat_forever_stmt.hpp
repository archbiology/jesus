#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <memory>
#include <vector>

/**
 * repeat forever:
 *   say 'Holy, holy, holy is the Lord God almighty, who was, and who is, and who is to come.'
 */
class RepeatForeverStmt : public Stmt
{
public:
    std::vector<std::unique_ptr<Stmt>> body;

    RepeatForeverStmt(std::vector<std::unique_ptr<Stmt>> body) : body(std::move(body)) {}

    void accept(StmtVisitor &visitor) const override;
};
